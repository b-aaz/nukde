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


#include <assert.h>			/* for assert	*/
#include <dirent.h>                     /* for readdir, opendir, rewinddir*/
#include <fcntl.h>                      /* for open, O_RDONLY*/
#include <magic.h>                      /* for magic_error, magic_load, magi...*/
#ifdef DEBUG
	#include <stdio.h>                      /* for fprintf, printf, NULL, puts*/
#endif
#include <stdlib.h>                     /* for getenv, qsort_r, malloc*/
#include <string.h>                     /* for strcmp, strlen, memset, strcat*/
#include <sys/dirent.h>                 /* for dirent*/
#include <sys/event.h>                  /* for kqueue, EVFILT_VNODE, EV_ADD*/

#define FAILSAFEICON "../examples/icons/err.ff"
#define BGIMAGE "../examples/icons/bg.ff"
#define FAILSAFEICONIDX "../examples/icon.nucfg"
#define FAILSAFEOPENIDX "../examples/open.nucfg"


#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRLIBMAGIC
#define NUERRSYSEVENT
#define NUERRSTDLIB
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "../../colibs/err.h"           /* for die*/
#include "../../colibs/bool.h"     /* for false, true*/
#include "../../colibs/spawn.h"
#include "../../colibs/nuklear.h"  /* for nk_vec2, nk_button_label, nk_...*/
#include "GL/gl.h"                      /* for GLint, glClear, glViewport*/
#include "GL/glx.h"                     /* for GLXFBConfig, glXGetFBConfigAt...*/
#include "X11/X.h"                      /* for None, AllocNone, Button1Motio...*/
#include "X11/Xlib.h"                   /* for XFree, XSetErrorHandler, XSync*/
#include "X11/Xutil.h"                  /* for XVisualInfo*/

#define NK_XLIB_GL3_IMPLEMENTATION
#define NK_XLIB_LOAD_OPENGL_EXTENSIONS
#include "lib/fileinfo_type.h"          /* for fileinfo, thrd_icon_load_args*/
#include "lib/fileops.h"                /* for XWindow, dsk_dir, menu*/
#include "lib/icon-loader.h"            /* for load_image_open_resize, loadicon*/
#include "lib/icon-widget.h"            /* for draw_icon*/
#include "lib/nuklear_xlib_gl3.h"       /* for nk_x11_font_stash_begin, nk_x...*/
#include "lib/sortfiles.h"               /* for sortfiles*/


#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NUKMENU "../nukmenu/bin/nukmenu"
#define MENUINPUT "LBL:Menu\nremove\nopen\nsort by\n,name\n,size\n,type\n,modification date\n,creation date\n,access date\n"

#define MENUINPUTLEN 94
#define MAXMENUITEMLEN 4

struct grid_config
{
	unsigned int icon_width ;
	unsigned int min_l_pad ;
	unsigned int min_r_pad ;
	unsigned int icon_h_pad ;
	unsigned int icon_v_pad ;
	unsigned int icon_txt_pad ;
	unsigned int row_pad ;
};


enum items
{
	REMOVE = 0,
	OPEN = 1 ,
	ST_NAME = 3 ,
	ST_SIZE = 4 ,
	ST_TYPE = 5 ,
	ST_MODIFICATION_DATE = 6 ,
	ST_CREATION_DATE = 7 ,
	ST_ACCESS_DATE = 8 ,
	TOGGAC = 9,
	TOGGCFG = 10,
};
struct menu
{
	bool menu;
	bool spawn;
	bool isrunning;
	int fd [2];
	enum items selected;
};
struct id
{
	bool return_id;
	unsigned int num;
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
pid_t spawnmenu (int * fd, unsigned int dp, unsigned int x, unsigned int y, unsigned int hp, unsigned int vp, struct id id)
{
	unsigned char uintdigs = 3 * sizeof (unsigned int) +1  ;
	char * args [14];
	pid_t cpid;
	args[0] = NUKMENU ;
	args[1] = "-dp";
	args[3] = "-x";
	args[5] = "-y";
	args[7] = "-hp";
	args[9] = "-vp";
	args[2] = malloc (uintdigs);
	args[4] = malloc (uintdigs);
	args[6] = malloc (uintdigs);
	args[8] = malloc (uintdigs);
	args[10] = malloc (uintdigs);
	snprintf (args[2], uintdigs, "%u", dp) ;
	snprintf (args[4], uintdigs, "%u", x) ;
	snprintf (args[6], uintdigs, "%u", y) ;
	snprintf (args[8], uintdigs, "%u", hp) ;
	snprintf (args[10], uintdigs, "%u", vp) ;

	if (id.return_id)
	{
		args[11]="-id";
		args[12]=malloc (uintdigs);
		snprintf (args[12], uintdigs, "%u", id.num) ;
		args[13]=NULL;
	}
	else
	{
		args[11]=NULL;
	}

	cpid=spawn (args, fd, SPAWN_RW) ;
	free (args[2]);
	free (args[4]);
	free (args[6]);
	free (args[8]);
	free (args[10]);

	if (id.return_id)
	{
		free (args[12]);
	}

	return cpid;
}

struct XWindow
{
	Display * dpy;
	Window win;
	XVisualInfo * vis;
	Colormap cmap;
	XSetWindowAttributes swa;
	XWindowAttributes attr;
	GLXFBConfig fbc;
	Atom wm_delete_window;
	int width, height;
};
static int gl_err = nk_false;
static int gl_error_handler (Display * dpy, XErrorEvent * ev)
{
	NK_UNUSED (dpy);
	NK_UNUSED (ev);
	gl_err = nk_true;
	return 0;
}
static int has_extension (const char * string, const char * ext)
{
	const char * start, *where, *term;
	where = strchr (ext, ' ');

	if (where || *ext == '\0')
	{ return nk_false; }

	for (start = string;;)
	{
		where = strstr ( (const char *) start, ext);

		if (!where)
		{
			break;
		}

		term = where + strlen (ext);

		if (where == start || * (where - 1) == ' ')
		{
			if (*term == ' ' || *term == '\0')
			{ return nk_true; }
		}

		start = term;
	}

	return nk_false;
}

struct XWindow  opendisplay (void)
{
	struct XWindow  win;
	memset (&win, 0, sizeof (win));
	win.dpy = XOpenDisplay (NULL);

	if (!win.dpy)
	{
		die ("%s","Failed to open X display\n");
	}

	return win;
}

void checkglversion (Display * dpy)
{
	int glx_major, glx_minor;

	if (!glXQueryVersion (dpy, &glx_major, &glx_minor))
	{ die ("%s","[X11]: Error: Failed to query OpenGL version\n"); }

	if ( (glx_major == 1 && glx_minor < 3) || (glx_major < 1))
	{ die ("%s","[X11]: Error: Invalid GLX version!\n"); }
}

void findbestfb (struct XWindow * win,GLXFBConfig * fbc,int fb_count)
{
	int i;
	int fb_best = -1, best_num_samples = -1;

	for (i = 0; i < fb_count; ++i)
	{
		XVisualInfo * vi = glXGetVisualFromFBConfig (win->dpy, fbc[i]);

		if (vi)
		{
			int sample_buffer, samples;
			glXGetFBConfigAttrib (win->dpy, fbc[i], GLX_SAMPLE_BUFFERS, &sample_buffer);
			glXGetFBConfigAttrib (win->dpy, fbc[i], GLX_SAMPLES, &samples);

			if ( (fb_best < 0) || (sample_buffer && samples > best_num_samples))
			{ fb_best = i, best_num_samples = samples; }
		}
	}

	win->fbc = fbc[fb_best];
	XFree (fbc);
	win->vis = glXGetVisualFromFBConfig (win->dpy, win->fbc);
}
void creatwindow (struct XWindow * win)
{
	win->cmap = XCreateColormap (win->dpy, RootWindow (win->dpy, win->vis->screen), win->vis->visual, AllocNone);
	win->swa.colormap =  win->cmap;
	win->swa.background_pixmap = None;
	win->swa.border_pixel = 0;
	win->swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask| ButtonMotionMask |
		Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
		PointerMotionMask| StructureNotifyMask;
	win->height=DisplayHeight (win->dpy,DefaultScreen (win->dpy));
	win->width=DisplayWidth (win->dpy,DefaultScreen (win->dpy));
	win->win = XCreateWindow (win->dpy, RootWindow (win->dpy, win->vis->screen), 0, 0,
							  win->width,win->height, 0, win->vis->depth, InputOutput,
							  win->vis->visual, CWBorderPixel|CWColormap|CWEventMask, &win->swa);

	if (!win->win)
	{
		die ("%s","[X11]: Failed to create window\n");
	}

	XFree (win->vis);
	XStoreName (win->dpy, win->win, "desk");
	XMapWindow (win->dpy, win->win);
	win->wm_delete_window = XInternAtom (win->dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (win->dpy, win->win, &win->wm_delete_window, 1);
}

void createglctx (struct XWindow * xwin, GLXContext * glContext)
{
	typedef GLXContext (*glxCreateContext) (Display *, GLXFBConfig, GLXContext, Bool, const int *);
	int (*old_handler) (Display *, XErrorEvent *) = XSetErrorHandler (gl_error_handler);
	const char * extensions_str = glXQueryExtensionsString (xwin->dpy, DefaultScreen (xwin->dpy));
	glxCreateContext create_context = (glxCreateContext)
									  glXGetProcAddressARB ( (const GLubyte *) "glXCreateContextAttribsARB");
	gl_err = nk_false;

	if (!has_extension (extensions_str, "GLX_ARB_create_context") || !create_context)
	{
#ifdef DEBUG
		fprintf (stderr, "[X11]: glXCreateContextAttribARB() not found...\n");
#endif
#ifdef DEBUG
		fprintf (stderr, "[X11]: ... using old-style GLX context\n");
#endif
		*glContext = glXCreateNewContext (xwin->dpy, xwin->fbc, GLX_RGBA_TYPE, 0, True);
	}
	else
	{
		GLint attr[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};
		*glContext = create_context (xwin->dpy, xwin->fbc, 0, True, attr);
		XSync (xwin->dpy, False);

		if (gl_err || !*glContext)
		{
			attr[1] = 1;
			attr[3] = 0;
			gl_err = nk_false;
#ifdef DEBUG
			fprintf (stdout, "[X11] Failed to create OpenGL 3.0 context\n");
#endif
#ifdef DEBUG
			fprintf (stdout, "[X11] ... using old-style GLX context!\n");
#endif
			*glContext = create_context (xwin->dpy, xwin->fbc, 0, True, attr);
		}
	}

	XSync (xwin->dpy, False);
	XSetErrorHandler (old_handler);

	if (gl_err || !*glContext)
	{ die ("%s","[X11]: Failed to create an OpenGL context\n"); }

	glXMakeCurrent (xwin->dpy, xwin->win, *glContext);
}



void choosefb (Display * dpy, int * fb_count, GLXFBConfig   * * fbc)
{
	static GLint attr[] =
	{
		GLX_X_RENDERABLE,   True,
		GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,    GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
		GLX_RED_SIZE,       8,
		GLX_GREEN_SIZE,     8,
		GLX_BLUE_SIZE,      8,
		GLX_ALPHA_SIZE,     8,
		GLX_DEPTH_SIZE,     24,
		GLX_STENCIL_SIZE,   8,
		GLX_DOUBLEBUFFER,   True,
		None
	};
	*fbc = glXChooseFBConfig (dpy, DefaultScreen (dpy), attr,fb_count);

	if (! *fbc)
	{
		die ("%s","[X11]: Error: failed to retrieve framebuffer configuration\n");
	}
}
struct  nk_font * addfont (const char * font_path,float height, const struct  nk_font_config * cfg)

{
	struct nk_font_atlas * atlas;
	nk_x11_font_stash_begin (&atlas);

	if (font_path!=NULL)
	{
		struct nk_font * future = nk_font_atlas_add_from_file (atlas,font_path, height, cfg);
	}

	nk_x11_font_stash_end();
}

struct nk_context * init_window (struct XWindow * xwin, GLXContext ** glContext)
{
	struct  nk_context * ctx;
	int fb_count;
	GLXFBConfig * fbc;
	*xwin=opendisplay();
	checkglversion (xwin->dpy);
	choosefb (xwin->dpy,&fb_count,&fbc);
	findbestfb (xwin,fbc,fb_count);
	creatwindow (xwin) ;
	createglctx (xwin,&glContext);
	ctx = nk_x11_init (xwin->dpy, xwin->win);
	return ctx;
}

	struct sortby sb;
int main (void)
{
	struct XWindow xwin;
	struct nk_context * ctx;
	GLXContext glContext;
	int running = 1;
	struct nk_colorf bg;
	struct nk_image  bgimage;
	char * folder="/desktop";
	char * hpath;
	int kqid;
	struct nk_vec2 lastpos;
	struct dsk_dir desktop_dir;
	struct dirent * dir ;
	char * iconidx;
	char * openidx;
	struct menu menu = {0};
	struct fileinfo ** files;
	size_t fi=0;
	unsigned int fnum=0;
	struct grid_config grid_cfg;
	bool show_gridcfg_menu = false;
	time_t time_of_last_frame ;
	time_t time_of_now ;
#ifdef  DEBUG
	unsigned long long fps=0;
#endif 
	grid_cfg.icon_width = 100;
	grid_cfg.min_l_pad = 30;
	grid_cfg.min_r_pad = 30;
	grid_cfg.icon_h_pad = 20;
	grid_cfg.icon_v_pad = 20;
	grid_cfg.icon_txt_pad = 20;
	grid_cfg.row_pad = 10;
	ctx=init_window (&xwin,&glContext);
	addfont (NULL,0,0);
	magic_t magic_cookie_mime=0;
	magic_t magic_cookie_hr=0;
	kqid=kqueue();
	magic_cookie_mime = magic_open (MAGIC_MIME|MAGIC_PRESERVE_ATIME|MAGIC_SYMLINK|MAGIC_MIME_TYPE);
	magic_cookie_hr = magic_open (MAGIC_NONE);
#ifdef DEBUG
	puts ("Loading magic databases\n");
#endif
	magic_load (magic_cookie_mime,NULL);
	magic_load (magic_cookie_hr,NULL);
	iconidx=fileopentobuff (FAILSAFEICONIDX);
	openidx=fileopentobuff (FAILSAFEOPENIDX);
	hpath=getenv ("HOME");
	desktop_dir.d_path =malloc (strlen (hpath)+strlen (folder)+1);
	strcpy (desktop_dir.d_path,hpath);
	strcat (desktop_dir.d_path,folder);
	desktop_dir.d_open = open (desktop_dir.d_path, O_RDONLY);
	EV_SET (&desktop_dir.d_change,desktop_dir.d_open, EVFILT_VNODE,
			EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR,
			NOTE_WRITE,
			0, 0);
	desktop_dir.d=opendir (desktop_dir.d_path);

	if (desktop_dir.d)
	{
		while ( (dir=readdir (desktop_dir.d)) != NULL)
		{
			if (! (!strcmp (".",dir->d_name) ||!strcmp ("..",dir->d_name)))
			{
				fnum++;
			}
		}
	}
	else
	{
		die ("%s\n","No desktop folder");
	}

#ifdef DEBUG
	printf ("%d\n",fnum);
#endif
	files=malloc (fnum * sizeof (struct fileinfo *));
	rewinddir (desktop_dir.d);

	while ( (dir=readdir (desktop_dir.d)) != NULL)
	{
		if (! (!strcmp (".",dir->d_name) ||!strcmp ("..",dir->d_name)))
		{
			files[fi]=new_file (desktop_dir.d_path,dir->d_name,iconidx,magic_cookie_mime,magic_cookie_hr);
			fi++;
		}
	}

#ifdef DEBUG
	puts ("Loading icons");
#endif

	for (unsigned int i = 0; i<fnum ; i++)
	{
		start_thrd_for_icon (files,i,i);
	}

	sb.ac=1;
	sb.st=NAME;
	sortfiles (files,fnum,sb);
	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.94f, bg.a = 1.0f;
	bgimage=load_image_open_resize (BGIMAGE, xwin.width,xwin.height);

	while (running)
	{
		/*Input handling */
		XEvent evt;
		nk_input_begin (ctx);

		while (XPending (xwin.dpy))
		{
			XNextEvent (xwin.dpy, &evt);

			if (evt.type == ClientMessage)
			{
				goto cleanup;
			}

			if (XFilterEvent (&evt, xwin.win))
			{
				continue;
			}

			nk_x11_handle_event (&evt);
		}

		nk_input_end (ctx);
		ctx->style.window.padding = nk_vec2 (0,0);
		ctx->style.window.spacing = nk_vec2 (0,0);
		ctx->style.window.scrollbar_size = nk_vec2 (0,0);

		if (menu.spawn)
		{
			struct id id ;
			id.return_id=true;
			id.num=0;
			spawnmenu (menu.fd,0,0,0, 10, 10, id);
			fcntl (menu.fd[0],O_NONBLOCK);
			write (menu.fd[1], MENUINPUT, MENUINPUTLEN) ;

			if (sb.ac>0) {
			write (menu.fd[1],",ascending\n", 11) ;

			}
			else {
			write (menu.fd[1],",descending\n", 12) ;
				 
			}
			if (show_gridcfg_menu)
			{
				write (menu.fd[1],"disable", 7) ;
			}
			else
			{
				write (menu.fd[1],"enable", 6) ;
			}

			write (menu.fd[1]," gird menu\n\n",10) ;
			close (menu.fd[1]);
			menu.spawn=false ;
			menu.isrunning=true;
		}

		if (menu.isrunning)
		{
			{
				char rt ;
				char ch;
				char ret_str [3];
				char * pret_str = ret_str;
				unsigned int sif=0;

				while (true)
				{
					rt = read (menu.fd[0], &ch, 1) ;

					if (rt==0)
					{
						if (pret_str>ret_str)
						{
							*pret_str='\0';
#ifdef DEBUG
							puts (ret_str) ;
#endif
							menu.selected=strtouint (ret_str,NULL,10);

							switch (menu.selected)
							{
								case REMOVE :
									while (sif<fnum)
									{
										if (files[sif]->isselected==true)
										{
											remove (files[sif]->path);
										}

										sif++;
									}

									break;

								case OPEN :
									while (sif<fnum)
									{
										if (files[sif]->isselected==true)
										{
											launch (openidx,*files[sif]);
										}

										sif++;
									}

									break;

								case TOGGCFG :
									show_gridcfg_menu=!show_gridcfg_menu;
									break;
								case TOGGAC :
									sb.ac=-sb.ac;
									sortfiles (files,fnum,sb);
									break;
								default : 
									sb.st=menu.selected - 3 ; 
									sortfiles (files,fnum,sb);

							}
						}

						while (sif<fnum)
						{
							files[sif]->isselected=false;
							sif++;
						}

						menu.isrunning=false;
						break;
					}

					if (rt==1)
					{
						*pret_str=ch;
						++pret_str;
					}

					if (rt==-1)
					{
						break;
					}
				}
			}
		}

		if (nk_begin (ctx, "desk", nk_rect (0, 0,xwin.width,xwin.height),0))
		{
			
			struct nk_window * win;
			unsigned int row=0;
			unsigned int col;
			unsigned int max_colomns;
			float colomn_pad ;
			unsigned int  icon_num;
			struct nk_rect icon_rect ;
			time_of_last_frame = time_of_now;
			time_of_now = time(NULL);
#ifdef DEBUG  
			fps++;
#endif 
			if (time_of_last_frame!= time_of_now)
			{
				/* Code here runs every second . */
				files= updatefiles (desktop_dir,&fnum,kqid,files,iconidx,magic_cookie_mime,magic_cookie_hr);
#ifdef DEBUG 
				printf("fps: %llu\n",fps);
				fps=0;
#endif
			}
			win = ctx->current;
			nk_draw_image (&win->buffer,nk_rect (0,0,xwin.width,xwin.height),&bgimage,nk_rgb (255,255,255));
			max_colomns= (xwin.width- (grid_cfg.min_l_pad+grid_cfg.min_r_pad)) / (grid_cfg.icon_width+grid_cfg.icon_h_pad);
			colomn_pad = (float) ( (xwin.width- (grid_cfg.min_l_pad+grid_cfg.min_r_pad)) % (grid_cfg.icon_width+grid_cfg.icon_h_pad)) /2;
			if (ctx->input.keyboard.text_len==1 && *ctx->input.keyboard.text=='q')
			{
				die("%s\n", "Exit key bind pressed");
			}

			while (row< (fnum/max_colomns)+1)
			{
				col=0;

				while (col+ (row*max_colomns) <fnum && col<max_colomns)
				{
					icon_num=col+ (row*max_colomns);
					icon_rect = nk_rect ( (col* (grid_cfg.icon_h_pad+grid_cfg.icon_width)+grid_cfg.min_l_pad+colomn_pad),
										  (row* (grid_cfg.icon_width+grid_cfg.icon_v_pad+grid_cfg.icon_txt_pad))+grid_cfg.row_pad,
										  grid_cfg.icon_width,
										  grid_cfg.icon_width+grid_cfg.icon_txt_pad);

					if (files[icon_num]->icon_load_args.generateid)
					{
						generateid (files[icon_num]);
					};

					draw_icon (ctx,files[icon_num]->name, (files[icon_num]),openidx,icon_rect);

					if (nk_input_is_mouse_click_in_rect (&ctx->input,NK_BUTTON_RIGHT,icon_rect))
					{
						if (nk_input_is_key_down ( (&ctx->input),NK_KEY_SHIFT))
						{
							files[icon_num]->isselected=!files[icon_num]->isselected;
						}
						else
						{
							files[icon_num]->isselected=true;
							menu.spawn=true;
						}
					}

					if (nk_input_is_mouse_click_in_rect (&ctx->input,NK_BUTTON_MIDDLE,icon_rect))
					{
						files[icon_num]->isselected=!files[icon_num]->isselected;
					}

					if (files[icon_num]->isselected)
					{
						nk_stroke_rect (&win->buffer,icon_rect,0,2,nk_rgba (250,250,250,32));
						nk_fill_rect (&win->buffer,icon_rect,0,nk_rgba (0,0,255,32));
					}

					col++;
				}

				row++;
			}

			if (nk_input_has_mouse_click (&ctx->input,0))
			{
				lastpos.x=* (&ctx->input.mouse.pos.x);
				lastpos.y=* (&ctx->input.mouse.pos.y);
			}

			if (show_gridcfg_menu)
			{
				nk_layout_space_begin (ctx,NK_STATIC,xwin.height,14);
				nk_layout_space_push (ctx, nk_rect (0,xwin.height-120,90,30));
				nk_text (ctx,"icon_width",10,NK_TEXT_CENTERED) ;
				nk_layout_space_push (ctx, nk_rect (90,xwin.height-120,xwin.width-90,30));
				nk_slider_int (ctx,0,(int *)&grid_cfg.icon_width,100,1);
				nk_layout_space_push (ctx, nk_rect (0,xwin.height-90,90,30));
				nk_text (ctx,"min_r_pad",9,NK_TEXT_CENTERED) ;
				nk_layout_space_push (ctx, nk_rect (90,xwin.height-90,xwin.width/2-90,30));
				nk_slider_int (ctx,0,(int *)&grid_cfg.min_r_pad,100,1);
				nk_layout_space_push (ctx, nk_rect (xwin.width/2,xwin.height-90,90,30));
				nk_text (ctx,"icon_h_pad",10,NK_TEXT_CENTERED) ;
				nk_layout_space_push (ctx, nk_rect (xwin.width/2+90,xwin.height-90,xwin.width/2-90,30));
				nk_slider_int (ctx,0,(int *)&grid_cfg.icon_h_pad,100,1);
				nk_layout_space_push (ctx, nk_rect (0,xwin.height-60,90,30));
				nk_text (ctx,"icon_v_pad",10,NK_TEXT_CENTERED) ;
				nk_layout_space_push (ctx, nk_rect (90,xwin.height-60,xwin.width/2-90,30));
				nk_slider_int (ctx,0,(int *)&grid_cfg.icon_v_pad,100,1);
				nk_layout_space_push (ctx, nk_rect (xwin.width/2,xwin.height-60,90,30));
				nk_text (ctx,"icon_txt_pad",12,NK_TEXT_CENTERED) ;
				nk_layout_space_push (ctx, nk_rect (xwin.width/2+90,xwin.height-60,xwin.width/2-90,30));
				nk_slider_int (ctx,0,(int *)&grid_cfg.icon_txt_pad,100,1);
				nk_layout_space_push (ctx, nk_rect (0,xwin.height-30,90,30));
				nk_text (ctx,"row_pad",7,NK_TEXT_CENTERED) ;
				nk_layout_space_push (ctx, nk_rect (90,xwin.height-30,xwin.width/2-90,30));
				nk_slider_int (ctx,0,(int *)&grid_cfg.row_pad,100,1);
				nk_layout_space_push (ctx, nk_rect (xwin.width/2,xwin.height-30,90,30));
				nk_text (ctx,"min_l_pad",9,NK_TEXT_CENTERED) ;
				nk_layout_space_push (ctx, nk_rect (xwin.width/2+90,xwin.height-30,xwin.width/2-90,30));
				nk_slider_int (ctx,0,(int *)&grid_cfg.min_l_pad,100,1);
				nk_layout_space_end (ctx);
			}
		}

		nk_end (ctx);
		XGetWindowAttributes (xwin.dpy, xwin.win, &xwin.attr);
		glClear (GL_COLOR_BUFFER_BIT);
		nk_x11_render (NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		glXSwapBuffers (xwin.dpy, xwin.win);
	}

cleanup:
	nk_x11_shutdown();
	glXMakeCurrent (xwin.dpy, 0, 0);
	glXDestroyContext (xwin.dpy, glContext);
	XUnmapWindow (xwin.dpy, xwin.win);
	XFreeColormap (xwin.dpy, xwin.cmap);
	XDestroyWindow (xwin.dpy, xwin.win);
	XCloseDisplay (xwin.dpy);
	return 0;
	return 0;
}
