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

	while (!feof (stdin))
	{
		fread (&character, 1, 1, stdin);

		if (buffersize >= allocatedsize)
		{
			allocatedsize *= 2;
			buffer = realloc (buffer, allocatedsize);
		}

		buffer[buffersize] = character;
		buffersize++;
	}

	buffer = realloc (buffer, buffersize+1);
	buffer[buffersize] = '\0';
	printf ("\n(:%s:)\n",buffer);
	return buffer;
}
struct menu_item * parse_buffer (char * buffer,size_t * item_count)
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
	while (true)
	{
		if (buffer[i]=='='&&current->type==IMG)
		{
			current->namel= (buffer+i)-current->name-1;
			buffer[i]='\0';
			i++;
			current->icpath=buffer+i;
		}

		if (last_char_was_newline)
		{
			if (buffer[i]!=',')
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

					/*Checking if we reached the end of buffer*/
					if (buffer[i]=='\0')
					{
						break;
					}
					if (buffer[i]=='\n')
					{
						goto skip;
					}

					/*If the current type is a label move it to the start of the list */
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

				/*Checking if item has a type to detect*/
				if (buffer[i+3]==':')
				{
					current->type=detect_type (buffer+i);
					i+=4;
				}
				else
				{
					current->type=NRM;
				}

				current->name=buffer+i;
				++*item_count;
			}
			else
			{

				if (current->type!=MOR)
				{
				current->namel= (buffer+i)-current->name;
					current->submenu_text=buffer+i;
				}

				current->type=MOR;
			}
		}

		if (buffer[i]=='\n')
		{
			last_char_was_newline=true;
			buffer[i]='\0';
		}
		else
		{
			last_char_was_newline=false;
		}
skip:
		i++;
	}

	/*Closing the end of list*/
	current->next=NULL;
	return head;
}

int main (void)
{
	long dt;
	long started;
	int running = 1;
	int i=0;
	size_t num=8;
	XWindow xw;
	struct nk_context * ctx;
	struct menu_item * current ;
	struct menu_item * head;
	float max_len=0;
	float width;
	float w,h;
	current = parse_buffer (getstdin(), &num);
	head=current;
	/* X11 */
	memset (&xw, 0, sizeof xw);
	xw.dpy = XOpenDisplay (NULL);

	if (!xw.dpy)
	{
		die ("Could not open a display; perhaps $DISPLAY is not set?");
	}

	xw.font = nk_xfont_create (xw.dpy, "fixed");

	while (true)
	{
		width=  nk_xfont_get_text_width (nk_handle_ptr (xw.font), (float) xw.font->height,current->name,current->namel);

		if (width > max_len)
		{
			max_len = width ;
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
	w=max_len;
	h=num*10;
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
	xw.win = XCreateWindow (xw.dpy, xw.root, 0, 0, w, h, 0,
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
	ctx->style.button.padding.x=0;
	ctx->style.window.padding.x=0;
	ctx->style.window.spacing.x=0;
	ctx->style.window.spacing.y=0;
	ctx->style.window.padding.y=0;
	ctx->style.button.padding.y=0;
	ctx->style.button.rounding=0;
	ctx->style.window.scrollbar_size.x=0;
	ctx->style.window.scrollbar_size.y=0;
	ctx->style.window.min_row_height_padding=0;

	while (running)
	{
		i=0;
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
		if (nk_begin (ctx, "Demo", nk_rect (0, 0,w,h),
					  0))
		{
			nk_layout_row_dynamic (ctx, 10, 1);

			while (true)
			{
				if (nk_button_text (ctx,current->name,current->namel-1))
				{puts (current->name); exit (EXIT_SUCCESS);}

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

		if (nk_window_is_hidden (ctx, "Demo"))
		{
			break;
		}

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

