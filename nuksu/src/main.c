/*
Copyright 2022, 2023 B-aaz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
 any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <sys/timespec.h>
#include "./lib/auth.h"
#include <X11/cursorfont.h>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_XLIB_IMPLEMENTATION
#define NK_ASSERT 
#define INCLUDE_STYLE 
/*#define NK_INPUT_MAX 2 */
#include "../../colibs/nuklear.h"
#include "../../colibs/bool.h"
#include "../../colibs/nuklear_xlib.h"
#include "./lib/widgets.h"
#define DTIME          20
#define WINDOW_WIDTH    200
#define WINDOW_HEIGHT   110
#define CHANCES   3
/*#define SHIFT 5*/
#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#define NUERRCOLORRE "\e[0m"
#define NUERRCOLOR "\e[38;2;237;67;55;1;5m"
#define WARN_COLOR "\e[38;2;255;121;0;2m"
#include "../../colibs/err.h"
#define DELAY   30
#define WARNFILE_LOCATION "/usr/share/security/nuksu"
#define RESTC "\e[0m"
#ifdef INCLUDE_STYLE
	#include "./style.c"
#endif

static long timestamp (void)
{
	struct timeval tv;

	if (gettimeofday (&tv, NULL) < 0)
	{
		return 0;
	}

	return (long) ( (long) tv.tv_sec * 1000 + (long) tv.tv_usec / 1000);
}
static void sleep_for (long t)
{
	struct timespec req;
	const time_t sec = (int) (t / 1000);
	const long ms = t - (sec * 1000);
	req.tv_sec = sec;
	req.tv_nsec = ms * 1000000L;

	while (-1 == nanosleep (&req, &req));
}
int main (int argc,char * argv[])
{
	long dt;
	long started;
	struct nk_context * ctx;
	bool running = true;
	bool warn=false;
	time_t utime=0;
	short int tries=0;
	struct XWindow xw;
	struct password password;
	FILE * init;
	struct passwords_input_data pd ;
	pd.active=0;
	pd.lockopeness=0;
	pd.lastt=0;
	pd.shift=0;
	pd.cursor_pos=0;
	pd.showpassword=0;

	if (argc==1)
	{
		die ("%s\n","No programs specified ");
	}

	if (access (WARNFILE_LOCATION,F_OK))
	{
		init=fopen (WARNFILE_LOCATION,"w");

		if (init==NULL)
		{
			die ("%s\n","Can not open the warning file");
		}

		fclose (init);
	}

	password.bufsize = 2;
	password.length=0;
	password.buf = malloc (password.bufsize * sizeof (char));
	memset (&xw, 0, sizeof xw);
	xw.dpy = XOpenDisplay (NULL);

	if (!xw.dpy)
	{ die ("%s\n","Could not open a display; perhaps $DISPLAY is not set?"); }

	xw.root = DefaultRootWindow (xw.dpy);
	xw.screen = XDefaultScreen (xw.dpy);
	xw.vis = XDefaultVisual (xw.dpy, xw.screen);
	xw.cmap = XCreateColormap (xw.dpy, xw.root, xw.vis, AllocNone);
	xw.swa.colormap = xw.cmap;
	xw.swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask | ButtonMotionMask |
		Button1MotionMask | Button3MotionMask | Button4MotionMask |
		Button5MotionMask |
		PointerMotionMask | KeymapStateMask;
	xw.win = XCreateWindow (xw.dpy, xw.root, 0, 0, WINDOW_WIDTH,
							WINDOW_HEIGHT, 0,
							XDefaultDepth (xw.dpy, xw.screen), InputOutput,
							xw.vis, CWEventMask | CWColormap, &xw.swa);
	XStoreName (xw.dpy, xw.win, "X11");
	XMapWindow (xw.dpy, xw.win);
	xw.wm_delete_window = XInternAtom (xw.dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (xw.dpy, xw.win, &xw.wm_delete_window, 1);
	XGetWindowAttributes (xw.dpy, xw.win, &xw.attr);
	xw.width = (unsigned int) xw.attr.width;
	xw.height = (unsigned int) xw.attr.height;
	xw.font = nk_xfont_create (xw.dpy, "fixed");
	ctx = nk_xlib_init (xw.font, xw.dpy, xw.screen, xw.win, xw.width,
						xw.height);
#ifdef INCLUDE_STYLE
	set_style (ctx, THEME_RED);
#endif

	while (running)
	{
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
		XGetWindowAttributes (xw.dpy, xw.win, &xw.attr);
		xw.width = (unsigned int) xw.attr.width;
		xw.height = (unsigned int) xw.attr.height;

		if (nk_begin (ctx, "nuksu", nk_rect (0, 0,xw.width,xw.height),0))
		{
			struct nk_window * win;
			struct nk_style * style;
			time_t timenow;
			struct warning_style warning_style;
			struct passwords_input_style  pds;
			char warning [100];
			FILE * warnfile;
			Cursor cu;
			cu = XCreateFontCursor (xw.dpy,XC_xterm);
			win = ctx->current;
			style = &ctx->style;
			warning_style.background = nk_rgb (255,40,24);
			warning_style.rounding = 4;
			warning_style.text_forground= nk_rgb(0,0,0);
			warning_style.text_background = warning_style.background;
			nk_layout_row_dynamic (ctx, 30, 1);
			pds.cursor_color=nk_rgb (255,255,255);
			pds.background_color=nk_rgb (68,71,90);
			pds.fild_inactivecolor=nk_rgb (200, 150, 100);
			pds.fild_activecolor=nk_rgb (100, 150, 200);
			pds.pass_textcolor=nk_rgb (255,255,255);
			pds.pass_backgroundcolor=nk_rgb (68,71,90);
			pds.label_textcolor=nk_rgb (0,0,0);
			pds.label_backgroundcolor=nk_rgb (68,71,90);
			pds.text_arias_start_padding=7;
			pds.space_betwen_charecters=6;
			pds.lockstyle.lock_speed=10;
			pds.lockstyle.shackle=nk_rgb (255,215,0);
			pds.lockstyle.body=nk_rgb (0,0,0);
			pds.lockstyle.key_hole=nk_rgb (255,255,255);
			pds.label="Enter password";
			password_input (ctx,&pd,pds,&password,xw,cu);
			nk_layout_row_dynamic (ctx, 25, 1);
			time (&timenow);

			if (nk_button_label (ctx, "Go!") || nk_input_is_key_pressed (&ctx->input, NK_KEY_ENTER))
			{
				warnfile=fopen (WARNFILE_LOCATION,"r");

				if (warnfile==NULL)
				{
					die ("%s\n","Can not open the warning file");
				}

				fscanf (warnfile,"%hd\n%ld",&tries,&utime);
				fclose (warnfile);

				if ( (timenow-utime) >DELAY)
				{
					if (auth (password.buf,"/etc/master.passwd"))
					{
						execvp (argv[1],argv+1);
						die ("%s\n","Can not execute the program");
					}
					else
					{
						puts (WARN_COLOR"Wrong password"RESTC);
						warn=true;
						tries++;

						if (tries>=CHANCES)
						{
							tries=0;
							utime=timenow;
						}

						warnfile=fopen (WARNFILE_LOCATION,"w");

						if (warnfile==NULL)
						{
							die ("%s\n","Can not open the warning file");
						}

						fprintf (warnfile,"%d\n%ld",tries,utime);
						fclose (warnfile);
					}
				}
				else
				{
					warn=true;
				}
			}

			if (warn)
			{
				if (CHANCES-tries==CHANCES&&timenow-utime<DELAY)
				{
					snprintf (warning,100,"Too meany tries wait for %lds",DELAY+utime-timenow);
				}
				else
				{
					snprintf (warning,100,"Wrong password %d more tries",CHANCES-tries);
				}

				nk_layout_row_dynamic (ctx, 25, 1);
				warning_widget (ctx ,warning_style ,warning);
			}
		}

		nk_end (ctx);
		XClearWindow (xw.dpy, xw.win);
		nk_xlib_render (xw.win, nk_rgba (20, 30, 30, 0));
		XFlush (xw.dpy);
		dt = timestamp() - started;

		if (dt < DTIME)
		{ sleep_for (DTIME - dt); }
	}

cleanup:
	free (password.buf);
	nk_xfont_del (xw.dpy, xw.font);
	nk_xlib_shutdown();
	XUnmapWindow (xw.dpy, xw.win);
	XFreeColormap (xw.dpy, xw.cmap);
	XDestroyWindow (xw.dpy, xw.win);
	XCloseDisplay (xw.dpy);
	return 0;
}
