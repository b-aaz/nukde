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

#include <X11/cursorfont.h>             /* for XC_xterm */
#include <stdio.h>                      /* for fclose, snprintf, fprintf */
#include <stdlib.h>                     /* for NULL, free, malloc */
#include <string.h>                     /* for memset */
#include <sys/time.h>                   /* for gettimeofday, timeval */
#include <sys/unistd.h>                 /* for F_OK */
#include <time.h>                       /* for nanosleep, time, time_t, time... */
#include <unistd.h>                     /* for access, execvp, NULL */

#include "./lib/auth.h"                 /* for auth */
#include "../../colibs/bool.h"     /* for true, bool, false */

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
/* #define INCLUDE_STYLE */
#include "../../colibs/nuklear.h"
#include "../../colibs/nuklear_xlib.h"  /* for XWindow, nk_xfont_create, nk_... */
#include "./lib/widgets.h"              

#define DTIME          20
#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 110
#define CHANCES   3
#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#define NUERRCOLORRE "\x1B[0m"
#define NUERRCOLOR "\x1B[38;2;237;67;55;1;5m"
#define WARN_COLOR "\x1B[38;2;255;121;0;2m"
#include "../../colibs/err.h"           /* for die, fopen */

#define DELAY   30
#define LOCKFILE_LOCATION "/usr/share/security/nuksu"
#define RESTC "\x1B[0m"
#define INCLUDE_STYLE
#ifdef INCLUDE_STYLE
	#include "./style.c"                    /* for set_style, theme */
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
void read_lock_file (short int * attempts, time_t * last_exceeding_time)
{
	FILE * lock_file;
	lock_file=fopen (LOCKFILE_LOCATION,"r");

	if (lock_file==NULL)
	{
		die ("%s\n","Can not open the lock file");
	}

	fscanf (lock_file,"%hd\n%ld",attempts,last_exceeding_time);
	fclose (lock_file);
}
void write_lock_file (short int attempts, time_t  last_exceeding_time)
{
	FILE * lock_file;
	lock_file=fopen (LOCKFILE_LOCATION,"w");

	if (lock_file==NULL)
	{
		die ("%s\n","Can not open the lock file");
	}

	fprintf (lock_file,"%d\n%ld",attempts,last_exceeding_time);
	fclose (lock_file);
}

void warning_default_style (struct warning_style * warning_style) {
			warning_style->bgcolor = nk_rgb (255,40,24);
			warning_style->rounding = 4;
			warning_style->text_color= nk_rgb (0,0,0);
			warning_style->text_bgcolor = warning_style->bgcolor;
}
void pass_edit_default_style (struct pass_edit_style * pds) {
			pds->cursor_color=nk_rgb (255,255,255);
			pds->background_color=nk_rgb (68,71,90);
			pds->fild_inactivecolor=nk_rgb (200, 150, 100);
			pds->fild_activecolor=nk_rgb (100, 150, 200);
			pds->pass_textcolor=nk_rgb (255,255,255);
			pds->pass_bgcolor=nk_rgb (68,71,90);
			pds->hint_bgcolor=nk_rgb (68,71,90);
			pds->hint_textcolor=nk_rgb(30,30,30);
			pds->text_aria_left_margin=7;
			pds->em_width=6;
			pds->lock_style.lock_speed=10;
			pds->lock_style.shackle=nk_rgb (255,215,0);
			pds->lock_style.body=nk_rgb (0,0,0);
			pds->lock_style.key_hole=nk_rgb (255,255,255);
}

int main (int argc,char * argv[])
{
	long dt;
	long started;
	struct nk_context * ctx;
	bool running = true;
	bool warn=false;
	time_t last_exceeding_time=0;
	short int attempts=0;
	struct XWindow xw;
	struct alstr password;
	struct pass_state pd ;
	char warning [100];
	time_t timeofnow;
	memset (&pd,0,sizeof (pd));
	memset (&password,0,sizeof (password));
	pd.active=true;

	if (argc==1)
	{
		die ("%s\n","No programs specified ");
	}

	read_lock_file (&attempts, &last_exceeding_time);
			time (&timeofnow);
	if (timeofnow-last_exceeding_time < DELAY){
		 warn=true;
	}
	password.allocated = 2;
	password.length=0;
	password.str = malloc (password.allocated * sizeof (char));

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
			bool newsecond;
			time_t timeoflastframe;
			struct warning_style warning_style;
			struct pass_edit_style  pds;
			char * hintstr = "Enter password";
			enum auth_return auth_return_val;
			Cursor cu;
			cu = XCreateFontCursor (xw.dpy,XC_xterm);
			pass_edit_default_style(&pds);
			warning_default_style(&warning_style);
			nk_layout_row_dynamic (ctx, 30, 1);
			pass_edit(ctx,&pd,pds,&password,hintstr,xw,cu);
			timeoflastframe = timeofnow ;
			time (&timeofnow);

			if (timeofnow != timeoflastframe)
			{
				newsecond = true ;
			}
			else
			{
				newsecond = false ;
			}

			nk_layout_row_dynamic (ctx, 25, 1);

			if (nk_button_label (ctx, "Go!") || nk_input_is_key_pressed (&ctx->input, NK_KEY_ENTER))
			{
				read_lock_file (&attempts, &last_exceeding_time);

				if ( (timeofnow-last_exceeding_time) >DELAY)
				{
					if (attempts!=0)
					{
						write_lock_file (0,  last_exceeding_time);
					}

					auth_return_val =auth (password.str,"/etc/master.passwd");

					switch (auth_return_val)
					{
						case AUTH_NOPASS :
							puts (WARN_COLOR"Account has no password"RESTC);
						case AUTH_SUCCESSES:
							execvp (argv[1],argv+1);
							die ("%s\n","Can not execute the program");
							break;

						case AUTH_FAIL:
							puts (WARN_COLOR"Wrong password"RESTC);
							warn=true;
							attempts++;

							if (attempts>=CHANCES)
							{
								attempts=0;
								last_exceeding_time=timeofnow;
							}

							write_lock_file (attempts,  last_exceeding_time);
							snprintf (warning,100,"Wrong password %d more attempts",CHANCES-attempts);
							break;
						case AUTH_LOCKED :
							puts (WARN_COLOR"Account is locked"RESTC);
							warn=true;
							snprintf (warning,100,"Account is locked");
							break;
						case AUTH_NOLOGIN :
							puts (WARN_COLOR"Account does not allow logins"RESTC);
							warn=true;
							snprintf (warning,100,"Account does not allow logins");
							break;
					}
				}
				else
				{
					warn=true;
						snprintf (warning,100,"Too many attempts wait for %lds",DELAY+last_exceeding_time-timeofnow);
				}
			}

			if (warn)
			{
				if (newsecond)
				{
					if (timeofnow-last_exceeding_time<DELAY)
					{
						snprintf (warning,100,"Too many attempts wait for %lds",DELAY+last_exceeding_time-timeofnow);
					}

					if (timeofnow-last_exceeding_time==DELAY)
					{
						warn = false;
					}
				}

				nk_layout_row_dynamic (ctx, 25, 1);
				warning_widget (ctx,warning_style,warning);
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
	free (password.str);
	nk_xfont_del (xw.dpy, xw.font);
	nk_xlib_shutdown();
	XUnmapWindow (xw.dpy, xw.win);
	XFreeColormap (xw.dpy, xw.cmap);
	XDestroyWindow (xw.dpy, xw.win);
	XCloseDisplay (xw.dpy);
	return 0;
}
