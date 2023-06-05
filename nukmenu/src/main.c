/* nuklear - v1.32.0 - public domain */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include<stdbool.h>
#include<string.h>
#define BUF_INIT_SIZE 256

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION
#include "../../colibs/nuklear.h"
#include "../../colibs/err.h"
#include "../../colibs/nuklear_xlib.h"

#define DTIME           20

typedef struct XWindow XWindow;
struct XWindow
{
	Display * dpy;
	Window root;
	Visual * vis;
	Colormap cmap;
	XWindowAttributes attr;
	XSetWindowAttributes swa;
	Window win;
	int screen;
	XFont * font;
	unsigned int width;
	unsigned int height;
	Atom wm_delete_window;
};

static void die (const char * fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	fputs ("\n", stderr);
	exit (EXIT_FAILURE);
}

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

#ifdef INCLUDE_ALL
	#define INCLUDE_STYLE
	#define INCLUDE_CALCULATOR
	#define INCLUDE_CANVAS
	#define INCLUDE_OVERVIEW
	#define INCLUDE_NODE_EDITOR
#endif
void print_chars ( char *  str , size_t strl ) 
{
	size_t i = 0 ;
	printf  ("<START>\n_______\n");
	while ( i < strl ) 
	{
		switch (str[i])
		{
		case '\n' :
			printf("<newline>");
			break ;
		case '\0' :
			printf("<NULL>");
			break ;
		case '\t' :
			printf("<Tab>");
			break ;
		case ' ' :
			printf("<Space>");
			break ;
		
		default :
			printf("%c" , str[i] ); 
	}
		i++;

	}
	printf  ("\n_______\n<END>\n");
}
enum  item_type { NRM =0, LBL =1, IMG =2,MOR=3};
struct menu_item
{
	enum item_type type;
	size_t namel;
	char * name;
	int * icon;
	size_t icpathl;
	char * icpath;
	char * submenu_text;
	size_t submenu_textl;
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

	while (	fread (&character, 1, 1, stdin))
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
struct menu_item * parse_buffer (char * buffer,size_t * item_count, unsigned int depth)
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
				if (*item_count!=0)
				{
					switch (current->type)
					{
						case IMG:
							current->icpathl= (buffer+i)-current->icpath-1;
							break;

						case MOR:
							current->submenu_textl= (buffer+i)-current->submenu_text-1;
							break;

						default:
							current->namel= (buffer+i)-current->name-1;
					}

					/*If the current type is a label move the item to the start of the list */
					if (current->type==LBL)
					{
						current->next=head;
						head=current;
						current=prev;
					}

					/*Creating a new node*/
					current->next=malloc (sizeof (struct menu_item));
					prev=current;
					current=current->next;
				}

				if (buffer[i]=='\n')
				{
					do
					{
						i++;
					}
					while (buffer[i]=='\n');

					if (buffer[i]=='\0')
					{break;}
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
			last_char_was_newline=true;
			if ( current -> type != MOR ) 
			{
				buffer[i]='\0';
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
struct args
{
	unsigned int dp;
	unsigned int x;
	unsigned int y;
	unsigned int vp;
	unsigned int hp;
};
unsigned int  strtouint (char * str,char ** restrict endptr,int b)
{
	long ret;
	ret= strtoul (str,endptr,b);

	if (errno!=EINVAL&& errno!=ERANGE)
	{
		return (unsigned int) ret;
	}

	die ("Invalid number %s",str);
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
				die ("numenu \"[ -h , -x x-pos, -y y-pos, -hp horizontal-padding, -vp vertical-padding, -dp depth]\"");
				argc++;
			}
			else if (strncmp (argv[i], "dp",2) ==0)
			{
				if (i+1>=argc)
				{
					die ("Option -%s requires a argument",argv[i]);
				}

				args.dp = strtouint (argv[i+1],NULL,10);
				i+=2;
			}
			else if (strncmp (argv[i], "x",1) ==0)
			{
				if (i+1>=argc)
				{
					die ("Option -%s requires a argument",argv[i]);
				}

				args.x = strtouint (argv[i+1],NULL,10);
				i+=2;
			}
			else if (strncmp (argv[i], "y",1) ==0)
			{
				if (i+1>=argc)
				{
					die ("Option -%s requires a argument",argv[i]);
				}

				args.y = strtouint (argv[i+1],NULL,10);
				i+=2;
			}
			else if (strncmp (argv[i], "vp",2) ==0)
			{
				if (i+1>=argc)
				{
					die ("Option -%s requires a argument",argv[i]);
				}

				args.vp = strtouint (argv[i+1],NULL,10);
				i+=2;
			}
			else if (strncmp (argv[i], "hp",2) ==0)
			{
				if (i+1>=argc)
				{
					die ("Option -%s requires a argument",argv[i]);
				}

				args.hp = strtouint (argv[i+1],NULL,10);
				i+=2;
			}
			else
			{
				die ("Unknown argument -%s \n", argv[i]) ;
			}
		}
		else
		{
			die ("Invalid argument %s \n", argv[i]) ;
		}
	}

	return args;
}

bool lunchsubmenu (unsigned int x,unsigned int y,unsigned int depth,char * submenutext, size_t submenutextl)
{
	FILE * child_stdin;
	char * progstr;
	fflush(NULL);
	sprintf ( progstr , "./bin/nukmenu -dp %u",depth+1 ); 	
	child_stdin = popen (progstr, "w") ;
	fwrite (submenutext,  submenutextl,1,child_stdin) ;
	fflush(child_stdin);
	pclose (child_stdin);
	return 1;
}
int main (int argc, char * * argv)
{
	long dt;
	long started;
	int running = 1;
	size_t num=8;
	XWindow xw;
	struct nk_context * ctx;
	struct menu_item * current ;
	struct menu_item * head;
	float max_text_width=0;
	float width;
	float rowheight;
	float w,h;
	struct nk_color;
	struct args args ;
	args=parse_args (argc,argv);
	current = parse_buffer (getstdin(), &num, args.dp);
	head=current;
	/* X11 */
	memset (&xw, 0, sizeof xw);
	xw.dpy = XOpenDisplay (NULL);

	if (!xw.dpy)
	{
		die ("Could not open a display; perhaps $DISPLAY is not set?");
	}

	xw.font = nk_xfont_create (xw.dpy, "fixed");
	max_text_width = find_max_text_width (head, xw.font);
	rowheight=xw.font->height+args.vp;
	w=max_text_width+args.hp;
	h=num*rowheight;
	xw.root = DefaultRootWindow (xw.dpy);
	xw.screen = XDefaultScreen (xw.dpy);
	xw.vis = XDefaultVisual (xw.dpy, xw.screen);
	xw.cmap = XCreateColormap (xw.dpy,xw.root,xw.vis,AllocNone);
	xw.swa.colormap = xw.cmap;
	xw.swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask| ButtonMotionMask |
		Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
		PointerMotionMask | KeymapStateMask;
	xw.win = XCreateWindow (xw.dpy, xw.root,args.x,args.y, w, h, 0,
							XDefaultDepth (xw.dpy, xw.screen), InputOutput,
							xw.vis, CWEventMask | CWColormap, &xw.swa);
	XStoreName (xw.dpy, xw.win, "X11");
	XMapWindow (xw.dpy, xw.win);
	xw.wm_delete_window = XInternAtom (xw.dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (xw.dpy, xw.win, &xw.wm_delete_window, 1);
	XGetWindowAttributes (xw.dpy, xw.win, &xw.attr);
	xw.width = (unsigned int) xw.attr.width;
	xw.height = (unsigned int) xw.attr.height;
	/* GUI */
	ctx = nk_xlib_init (xw.font, xw.dpy, xw.screen, xw.win, xw.width, xw.height);
#ifdef INCLUDE_STYLE
	/* ease regression testing during Nuklear release process; not needed for anything else */
#ifdef STYLE_WHITE
	set_style (ctx, THEME_WHITE);
#elif defined(STYLE_RED)
	set_style (ctx, THEME_RED);
#elif defined(STYLE_BLUE)
	set_style (ctx, THEME_BLUE);
#elif defined(STYLE_DARK)
	set_style (ctx, THEME_DARK);
#endif
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

			if (XFilterEvent (&evt, xw.win))
			{
				continue;
			}

			nk_xlib_handle_event (xw.dpy, xw.screen, xw.win, &evt);
		}

		nk_input_end (ctx);

		/* GUI */
		if (nk_begin (ctx, "", nk_rect (0, 0,w,h),
					  0))
		{
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
							puts (current->name);
							exit (EXIT_SUCCESS);
						}
						else
						{
							if (lunchsubmenu (0,0,args.dp,current->submenu_text,current->submenu_textl))
							{
								exit (EXIT_SUCCESS);
							}
						}
					}
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

