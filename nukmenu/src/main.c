/*
* Copyright (c) 2022-2023 B-aaz .  
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include<string.h>
#define BUF_INIT_SIZE 256

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION
#include "../../colibs/nuklear.h"
#include "./lib/bit.h"
#include "../../colibs/nuklear_xlib.h"
#define  NUERRSTDIO
#define  NUERRSTDLIB
#define  NUERRREDEFFUNCS
#include "../../colibs/err.h"
#include "../../colibs/bool.h"
#include "../../colibs/spawn.h"

#define DTIME           20


static long timestamp (void)
{
	struct timeval tv;

	if (gettimeofday (&tv, NULL) < 0)
	{
		return 0;
	}

	return (long) ( (long) tv.tv_sec * 1000 + (long) tv.tv_usec/1000);
}

static void sleep_for (long t)
{
	struct timespec req;
	const time_t sec = (int) (t/1000);
	const long ms = t - (sec * 1000);
	req.tv_sec = sec;
	req.tv_nsec = ms * 1000000L;

	while (-1 == nanosleep (&req, &req));
}

enum  item_type { NRM =0, LBL =1, IMG =2,MOR=3};
struct menu_item
{
	enum item_type type;
	unsigned int id;
	size_t submenu_textl;
	size_t namel;
	size_t icpathl;
	char * name;
	int * icon;
	char * icpath;
	char * submenu_text;
	struct menu_item * next ;
};
enum item_type detect_type (char * type)
{
	if (strncmp (type,"LBL",3) ==0)
	{
		return LBL;
	}

	if (strncmp (type,"IMG",3) ==0)
	{
		return IMG;
	}

	return NRM;
}
char * getstdin()
{
	char character;
	char * buffer = malloc (BUF_INIT_SIZE);
	size_t buffersize = 0;
	size_t allocatedsize = BUF_INIT_SIZE;

	while (fread (&character, 1, 1, stdin))
	{
		if (buffersize >= allocatedsize)
		{
			allocatedsize *= 2;
			buffer = realloc (buffer, allocatedsize);
		}

		buffer[buffersize] = character;
		buffersize++;
	}

	buffer = realloc (buffer, buffersize+3);
	buffer[buffersize] = '\n';
	buffer[buffersize+1] = '\n';
	buffer[buffersize+2] = '\0';
	return buffer;
}
enum argnnums
{
	ARGN_DP,
	ARGN_X,
	ARGN_Y,
	ARGN_VP,
	ARGN_HP,
	ARGN_ID,
};

static const char * argnames[6]= {"-dp","-x","-y","-vp","-hp","-id"};

union argsints
{
	struct
	{
		unsigned int dp; /* Number of sub-menus deep we are, Starts at 0 . */
		int x; /* Menu's window x position . */
		int y; /* Menu's window x position . */
		unsigned int vp; /* Padding around the left and right edges . */
		unsigned int hp; /* Padding around the left and right edges . */
		unsigned int id; /* Output the selected items id instead of its name . */
	};
	int a [sizeof (argnames) /sizeof (argnames[0])];
};

struct args
{
	union argsints v;
	unsigned char argset [1]; /* Each bit indicates if the argument is
				     given . */
};
void set_name_length (char * buffer,size_t offset, struct menu_item * item)
{
	switch (item->type)
	{
		case IMG:
			item->icpathl= (buffer+offset)-item->icpath-1;
			break;

		case MOR:
			item->submenu_textl= (buffer+offset)-item->submenu_text-1;
			break;

		default:
			item->namel= (buffer+offset)-item->name-1;
	}
}
struct menu_item * parse_input (char * buffer,size_t * item_count, unsigned int depth, unsigned int id,unsigned char * argset)
{
	struct menu_item * head;
	struct menu_item * prev;
	struct menu_item * current;
	size_t i=0;
	bool last_char_was_newline=true;
	current=malloc (sizeof (struct menu_item));
	/*Saving the list's head*/
	head = current ;
	*item_count=0;

	/*Going thru the buffer character by character*/

	while (buffer[i]!='\0')
	{
		if (last_char_was_newline)
		{
			if (buffer[i+depth]!=',')
			{
				if (current->type==LBL)
				{
					--id;
				}

				if (*item_count!=0)
				{
					set_name_length (buffer,i,current);

					/* Skips extra newlines . */
					if (buffer[i]=='\n')
					{
						i++;

						while (buffer[i]=='\n')
						{
							i++;
						}

						if (buffer[i]=='\0')
						{
							break;
						}
					}

					/*Creating a new node*/
					current->next=malloc (sizeof (struct menu_item));
					prev=current;
					current=current->next;
				}

				i+=depth;

				/*Checking if item has a type*/
				if (buffer[i+3]==':')
				{
					/*Detecting the type */
					current->type=detect_type (buffer+i);
					/*Skipping 4 Characters to get to the start of the name*/
					i+=4;
				}
				else
				{
					/*Setting type to normal*/
					current->type=NRM;
				}

				current->id=id;
				current->name=buffer+i;
				++*item_count;
			}
			else
			{
				if (current->type!=MOR)
				{
					current->namel= (buffer+i)-current->name-1;
					current->submenu_text=buffer+i;
				}

				current->type=MOR;
				last_char_was_newline=false;
				i++;
				continue;
			}
		}
		else
		{
			if (current->type==IMG&&buffer[i]=='=')
			{
				current->namel= (buffer+i)-current->name-1;
				buffer[i]='\0';
				i++;
				current->icpath=buffer+i;
				i++;
				continue;
			}
		}

		if (buffer[i]=='\n')
		{
			if (getbit (argset,ARGN_ID))
			{
				++id;
			}

			last_char_was_newline=true;

			if (current->type != MOR)
			{
				buffer[i]='\0';
				i++;
				continue;
			}
		}
		else
		{
			last_char_was_newline=false;
		}

		i++;
	}

	/*Closing the end of list*/
	current->next=NULL;
	return head;
}
float find_max_text_width (struct menu_item * head, XFont * font)
{
	struct menu_item * current = head;
	float max_text_width = 0 ;
	float width= 0 ;

	while (true)
	{
		width=  nk_xfont_get_text_width (nk_handle_ptr (font), (float) font->height,current->name,current->namel);

		if (width > max_text_width)
		{
			max_text_width = width ;
		}

		if (current->next!=NULL)
		{
			current=current->next;
		}
		else
		{
			break;
		}
	}

	current = head;
	return max_text_width;
}
unsigned int  strtouint (char * str,char ** restrict endptr,int b)
{
	long ret;
	ret= strtoul (str,endptr,b);

	if (errno!=EINVAL&& errno!=ERANGE)
	{
		return (unsigned int) ret;
	}

	die ("%s%s\n","Invalid number ",str);
	return 0;
}
struct args parse_args (int argc, char ** argv)
{
	struct args args= {0};
	int i = 1;

	while (i < argc)
	{
		if (* (argv[i]) =='-')
		{
			argv[i]++;

			if (strncmp (argv[i], "h",2) ==0)
			{
				die ("%s\n","nukmenu \"[ -h , -x x-pos, -y y-pos, -hp horizontal-padding, -vp vertical-padding, -dp depth]\"");
				argc++;
			}
			else if (strncmp (argv[i], "dp",2) ==0)
			{
				if (i+1>=argc)
				{
					die ("%s\n","Option -%s requires a argument",argv[i]);
				}

				args.v.dp = strtouint (argv[i+1],NULL,10);
				setbit (args.argset,ARGN_DP,1);
				i+=2;
			}
			else if (strncmp (argv[i], "x",1) ==0)
			{
				if (i+1>=argc)
				{
					die ("%s\n","Option -%s requires a argument",argv[i]);
				}

				args.v.x = strtouint (argv[i+1],NULL,10);
				setbit (args.argset,ARGN_X,1);
				i+=2;
			}
			else if (strncmp (argv[i], "y",1) ==0)
			{
				if (i+1>=argc)
				{
					die ("%s\n","Option -%s requires a argument",argv[i]);
				}

				args.v.y = strtouint (argv[i+1],NULL,10);
				setbit (args.argset,ARGN_Y,1);
				i+=2;
			}
			else if (strncmp (argv[i], "vp",2) ==0)
			{
				if (i+1>=argc)
				{
					die ("%s\n","Option -%s requires a argument",argv[i]);
				}

				args.v.vp = strtouint (argv[i+1],NULL,10);
				setbit (args.argset,ARGN_VP,1);
				i+=2;
			}
			else if (strncmp (argv[i], "hp",2) ==0)
			{
				if (i+1>=argc)
				{
					die ("%s\n","Option -%s requires a argument",argv[i]);
				}

				args.v.hp = strtouint (argv[i+1],NULL,10);
				setbit (args.argset,ARGN_HP,1);
				i+=2;
			}
			else if (strncmp (argv[i], "id",2) ==0)
			{
				if (i+1>=argc)
				{
					die ("%s\n","Option -%s requires a argument",argv[i]);
				}

				args.v.id=strtouint (argv[i+1],NULL,10);
				setbit (args.argset,ARGN_ID,1);
				i+=2;
			}
			else
			{
				die ("%s\n","Unknown argument -%s \n", argv[i]) ;
			}
		}
		else
		{
			die ("Invalid argument %s \n", argv[i]) ;
		}
	}

	return args;
}
pid_t spawnmenu (char * path, int * fd, struct args args)
{
	unsigned char uintdigs = 3 * sizeof (unsigned int) +1  ;
	unsigned char argsetbytes = sizeof (argnames) /sizeof (argnames[0]) /8+1;
	size_t setcount = countbitsset (args.argset, argsetbytes);
	unsigned char spwargsc = setcount*2;
	char * spwargs[spwargsc];
	pid_t cpid;
	spwargs[0]=path;
	spwargs[spwargsc-1]=NULL;

	for (unsigned char i=1; i<spwargsc-1; i++)
	{
		spwargs[i] = argnames[(i-1)/2];
		i++;
		spwargs[i] = alloca (uintdigs);
		snprintf (spwargs[i], uintdigs, "%u", args.v.a[(i-2)/2]) ;
	}

	cpid=spawn (spwargs, fd, SPAWN_RW) ;
	return cpid;
}
int launchsubmenu (char * path,char * submenutext, size_t submenutextl, struct args subargs)
{
	int  fd [2];
	char ch;
	int stat;
	spawnmenu (path,fd,subargs) ;
	write (fd[1],submenutext,  submenutextl) ;
	close (fd[1]) ;
	wait (&stat) ;

	while (read (fd[0], &ch, 1))
	{ write (1, &ch, 1) ; }

	return stat;
}
void getcursorpos (Display * dpy, Window root, int * x,int * y)
{
	/* Oh look at what we got here!!!
	 * 5 use less variables just to get a simple cursor position
	 */
	Window   useless_variable1;
	Window  useless_variable2;
	int  useless_variable3;
	int  useless_variable4;
	unsigned int  useless_variable5;
	XQueryPointer (dpy,root,&useless_variable1,&useless_variable2,x,y,&useless_variable3,&useless_variable4,&useless_variable5);
	/* if you know a better way to do this tell me */
}
struct menu_window
{
	int x;
	int y;
	unsigned int w;
	unsigned int h;
};
/* Disables the window manager decorations . */
void disable_border (struct XWindow xw)
{
	struct motif_hints
	{
		unsigned long flags;
		unsigned long funcs;
		unsigned long decor;
		long input_mode;
		unsigned long stats;
	};
	struct motif_hints hints;
	Atom property;
	hints.flags=2;
	hints.decor=0;
	property=XInternAtom (xw.dpy,"_MOTIF_WM_HINTS",False);
	XChangeProperty (xw.dpy,xw.win,property,property
					 ,32,PropModeReplace, (unsigned char *) &hints,5);
}
void reverse_list (struct menu_item ** head)
{
	struct menu_item * current=*head;
struct menu_item * tempnext= (*head);
	struct menu_item * prev=NULL;

	while (current!=NULL)
	{
		tempnext=current->next;
		current->next=prev;
		prev=current;
		current=tempnext;
	}

	*head=prev;
}
int main (int argc, char * * argv)
{
	long dt;
	long started;
	int running = 1;
	size_t num;
	struct XWindow xw;
	struct nk_context * ctx;
	struct menu_item * current ;
	struct menu_item * head;
	float max_text_width=0;
	float width;
	float rowheight;
	struct nk_color;
	struct args args= {0};
	unsigned int itemcounter;
	bool ignoreleave = false ;
	struct menu_window menuwin= {0};
	args=parse_args (argc,argv);
	current = parse_input (getstdin(), &num, args.v.dp, args.v.id,args.argset);
	head=current;
	/* X11 */
	memset (&xw, 0, sizeof xw);
	xw.dpy = XOpenDisplay (NULL);

	if (!xw.dpy)
	{
		die ("%s\n","Could not open a display; perhaps $DISPLAY is not set?");
	}

	xw.font = nk_xfont_create (xw.dpy, "fixed");
	max_text_width = find_max_text_width (head, xw.font);
	rowheight=xw.font->height+args.v.vp;
	menuwin.w=max_text_width+args.v.hp;
	menuwin.h=num*rowheight;
	xw.root = DefaultRootWindow (xw.dpy);
	xw.screen = XDefaultScreen (xw.dpy);
	xw.vis = XDefaultVisual (xw.dpy, xw.screen);
	xw.cmap = XCreateColormap (xw.dpy,xw.root,xw.vis,AllocNone);
	xw.swa.colormap = xw.cmap;
	xw.swa.event_mask =
		LeaveWindowMask|	ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask| ButtonMotionMask |
		Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
		PointerMotionMask | KeymapStateMask;

	if (args.v.dp==0)
	{
		unsigned int dw,dh;   /* Display width and height . */
		dh=DisplayHeight (xw.dpy,xw.screen);
		dw=DisplayWidth (xw.dpy,xw.screen);
		int px, py; /* Pointers x y coordinates . */
		getcursorpos (xw.dpy,xw.root,&px,&py);

		if (!getbit (args.argset,ARGN_X))
		{
			menuwin.x=px;
			/* Negatively offsetting the x position by the
			 * horizontal padding to put the window slightly under
			 * the cursor . */
			menuwin.x-=args.v.hp/2;

			if (menuwin.x+menuwin.w>dw)
			{
				menuwin.x-=menuwin.w;
				menuwin.x+=args.v.hp;
			}
		}
		else
		{
			menuwin.x=args.v.x;
		}

		if (!getbit (args.argset,ARGN_Y))
		{
			menuwin.y=py;
			menuwin.y-=args.v.vp/2;

			if (menuwin.y+menuwin.h>dh)
			{
				/* Reverses the list of items if the menu spawns on the
				 * top of cursor . */
				reverse_list (&head);
				menuwin.y-=menuwin.h;
				menuwin.y+=args.v.vp;
			}
		}
		else
		{
			menuwin.y=args.v.y;
		}
	}
	else
	{
		menuwin.x=args.v.x;
		menuwin.y=args.v.y;
	}

	xw.win = XCreateWindow (xw.dpy, xw.root,menuwin.x,menuwin.y, menuwin.w, menuwin.h, 0,
							XDefaultDepth (xw.dpy, xw.screen), InputOutput,
							xw.vis, CWEventMask | CWColormap, &xw.swa);
	XStoreName (xw.dpy, xw.win, "NukMenu");
	disable_border (xw);
	XMapWindow (xw.dpy, xw.win);
	xw.wm_delete_window = XInternAtom (xw.dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (xw.dpy, xw.win, &xw.wm_delete_window, 1);
	XGetWindowAttributes (xw.dpy, xw.win, &xw.attr);
	xw.width = (unsigned int) xw.attr.width;
	xw.height = (unsigned int) xw.attr.height;
	/* GUI */
	ctx = nk_xlib_init (xw.font, xw.dpy, xw.screen, xw.win, xw.width, xw.height);
#ifdef INCLUDE_STYLE
	set_style (ctx, THEME_RED);
#endif
	ctx->style.window.spacing.y=0;
	ctx->style.window.padding.y=0;
	ctx->style.window.spacing.x=0;
	ctx->style.window.padding.x=0;
	ctx->style.button.padding.y=0;
	ctx->style.button.padding.x=0;
	ctx->style.button.rounding=0;
	ctx->style.window.scrollbar_size.x=0;
	ctx->style.window.scrollbar_size.y=0;
	ctx->style.window.min_row_height_padding=0;

	while (running)
	{
		/* Input */
		XEvent evt;
		started = timestamp();
		nk_input_begin (ctx);

		while (XPending (xw.dpy))
		{
			XNextEvent (xw.dpy, &evt);

			if (evt.type == ClientMessage)
			{
				goto cleanup;
			}

			if (evt.type == LeaveNotify)
			{
				if (ignoreleave)
				{
					ignoreleave = false;
				}
				else
				{
					exit (EXIT_FAILURE);
				}
			}

			if (XFilterEvent (&evt, xw.win))
			{
				continue;
			}

			nk_xlib_handle_event (xw.dpy, xw.screen, xw.win, &evt);
		}

		nk_input_end (ctx);

		/* GUI */
		if (nk_begin (ctx, "", nk_rect (0, 0,menuwin.w,menuwin.h),
					  0))
		{
			itemcounter = 0 ;
			nk_layout_row_dynamic (ctx, rowheight, 1);

			while (true)
			{
				if (current->type == LBL)
				{
					nk_text (ctx,current->name,current->namel,NK_TEXT_CENTERED) ;
				}
				else
				{
					if (nk_button_text (ctx,current->name,current->namel))
					{
						if (current->type!=MOR)
						{
							if (getbit (args.argset,ARGN_ID))
							{
								printf ("%u\n", current->id);
							}
							else
							{
								puts (current->name);
							}

							exit (EXIT_SUCCESS);
						}
						else
						{
							struct args subargs={0};
							subargs.v.dp=args.v.dp+1;
							subargs.v.x=menuwin.x+menuwin.w;
							subargs.v.y=menuwin.y+itemcounter*rowheight;
							subargs.v.vp=args.v.vp;
							subargs.v.hp=args.v.hp;
							subargs.v.id=args.v.id;
							subargs.argset[0]= 63;
							/*We are going to launch a sub-menu*/
							ignoreleave=true;
							args.v.id=++current->id;

							if (
								WEXITSTATUS
								(
									launchsubmenu
									(
										argv[0],
										current->submenu_text,
										current->submenu_textl,
										subargs
									)
								)
								==EXIT_SUCCESS
							)
							{
								goto cleanup;
							}
						}
					}
				}

				if (current->next!=NULL)
				{
					current=current->next;
					++itemcounter;
				}
				else
				{
					break;
				}
			}

			current = head;
		}

		nk_end (ctx);
		/* Draw */
		XClearWindow (xw.dpy, xw.win);
		nk_xlib_render (xw.win, nk_rgb (30,30,30));
		XFlush (xw.dpy);
		/* Timing */
		dt = timestamp() - started;

		if (dt < DTIME)
		{ sleep_for (DTIME - dt); }
	}

cleanup:
	nk_xfont_del (xw.dpy, xw.font);
	nk_xlib_shutdown();
	XUnmapWindow (xw.dpy, xw.win);
	XFreeColormap (xw.dpy, xw.cmap);
	XDestroyWindow (xw.dpy, xw.win);
	XCloseDisplay (xw.dpy);
	return 0;
}

