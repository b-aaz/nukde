#include <assert.h>			/* for assert	*/
#include <dirent.h>                     /* for readdir, opendir, rewinddir*/
#include <fcntl.h>                      /* for open, O_RDONLY*/
#include <magic.h>                      /* for magic_error, magic_load, magi...*/
#include <stdio.h>                      /* for fprintf, printf, NULL, puts*/
#include <stdlib.h>                     /* for getenv, qsort_r, malloc*/
#include <string.h>                     /* for strcmp, strlen, memset, strcat*/
#include <sys/dirent.h>                 /* for dirent*/
#include <sys/event.h>                  /* for kqueue, EVFILT_VNODE, EV_ADD*/

#define FAILSAFEICON "../examples/pics/err.ff"
#define BGIMAGE "../examples/pics/bg.ff"
#define FAILSAFEICONIDX "../examples/icon.nucfg"
#define FAILSAFEOPENIDX "../examples/open.nucfg"

#define ICON_W 150
#define MINLFPAD 30
#define MINRIPAD 30
#define ICONHPAD 20

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
#include "../../colibs/nuklear.h"  /* for nk_vec2, nk_button_label, nk_...*/
#include "GL/gl.h"                      /* for GLint, glClear, glViewport*/
#include "GL/glx.h"                     /* for GLXFBConfig, glXGetFBConfigAt...*/
#include "X11/X.h"                      /* for None, AllocNone, Button1Motio...*/
#include "X11/Xlib.h"                   /* for XFree, XSetErrorHandler, XSync*/
#include "X11/Xutil.h"                  /* for XVisualInfo*/

#define NK_XLIB_GL3_IMPLEMENTATION
#define NK_XLIB_LOAD_OPENGL_EXTENSIONS
#include "lib/fileinfo_type.h"          /* for fileinfo, thrd_icon_load_args*/
#include "lib/fileops.h"                /* for XWindow, dsk_dir, menupos*/
#include "lib/icon-loader.h"            /* for load_image_open_resize, loadicon*/
#include "lib/icon-widget.h"            /* for draw_icon*/
#include "lib/nuklear_xlib_gl3.h"       /* for nk_x11_font_stash_begin, nk_x...*/
#include "lib/sortfiles.h"               /* for sortfiles*/


#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024



static int gl_err = nk_false;
static int gl_error_handler(Display * dpy, XErrorEvent * ev)
{
    NK_UNUSED(dpy);
    NK_UNUSED(ev);
    gl_err = nk_true;
    return 0;
}
static int has_extension(const char * string, const char * ext)
{
    const char * start, *where, *term;
    where = strchr(ext, ' ');
    if(where || *ext == '\0')
        return nk_false;
    for(start = string;;)
    {
        where = strstr((const char *)start, ext);
        if(!where) break;
        term = where + strlen(ext);
        if(where == start || *(where - 1) == ' ')
        {
            if(*term == ' ' || *term == '\0')
                return nk_true;
        }
        start = term;
    }
    return nk_false;
}



int main(void)
{
    unsigned int desktop_hight,desktop_width;
    struct nk_image  bgimage;
    int running = 1;
    struct XWindow win;
    GLXContext glContext;
    struct nk_context * ctx;
    struct nk_colorf bg;
    char * folder="/desktop";
    char * hpath;
    int kqid;
    struct nk_vec2 lastpos;
    struct dsk_dir desktop_dir;
    struct dirent * dir ;
    char * iconidx;
    char * openidx;
    magic_t magic_cookie_mime=0;
    magic_t magic_cookie_hr=0;
    kqid=kqueue();
    magic_cookie_mime = magic_open(MAGIC_MIME|MAGIC_PRESERVE_ATIME|MAGIC_SYMLINK|MAGIC_MIME_TYPE);
    magic_cookie_hr = magic_open(MAGIC_NONE);
magic_load(magic_cookie_mime,NULL);
magic_load(magic_cookie_hr,NULL);
    puts("Loading magic databases\n");

    memset(&win, 0, sizeof(win));
    win.dpy = XOpenDisplay(NULL);
    if(!win.dpy) die("%s","Failed to open X display\n");
    {
        int glx_major, glx_minor;
        if(!glXQueryVersion(win.dpy, &glx_major, &glx_minor))
            die("%s","[X11]: Error: Failed to query OpenGL version\n");
        if((glx_major == 1 && glx_minor < 3) || (glx_major < 1))
            die("%s","[X11]: Error: Invalid GLX version!\n");
    }
    {
        int fb_count;
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
        GLXFBConfig * fbc;
        fbc = glXChooseFBConfig(win.dpy, DefaultScreen(win.dpy), attr, &fb_count);
        if(!fbc) die("%s","[X11]: Error: failed to retrieve framebuffer configuration\n");
        {
            int i;
            int fb_best = -1, best_num_samples = -1;
            for(i = 0; i < fb_count; ++i)
            {
                XVisualInfo * vi = glXGetVisualFromFBConfig(win.dpy, fbc[i]);
                if(vi)
                {
                    int sample_buffer, samples;
                    glXGetFBConfigAttrib(win.dpy, fbc[i], GLX_SAMPLE_BUFFERS, &sample_buffer);
                    glXGetFBConfigAttrib(win.dpy, fbc[i], GLX_SAMPLES, &samples);
                    if((fb_best < 0) || (sample_buffer && samples > best_num_samples))
                        fb_best = i, best_num_samples = samples;
                }
            }
            win.fbc = fbc[fb_best];
            XFree(fbc);
            win.vis = glXGetVisualFromFBConfig(win.dpy, win.fbc);
        }
    }
    {
        win.cmap = XCreateColormap(win.dpy, RootWindow(win.dpy, win.vis->screen), win.vis->visual, AllocNone);
        win.swa.colormap =  win.cmap;
        win.swa.background_pixmap = None;
        win.swa.border_pixel = 0;
        win.swa.event_mask =
            ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPress | ButtonReleaseMask| ButtonMotionMask |
            Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
            PointerMotionMask| StructureNotifyMask;
        desktop_hight=DisplayHeight(win.dpy,DefaultScreen(win.dpy));
        desktop_width=DisplayWidth(win.dpy,DefaultScreen(win.dpy));
        win.win = XCreateWindow(win.dpy, RootWindow(win.dpy, win.vis->screen), 0, 0,
                                desktop_width, desktop_hight, 0, win.vis->depth, InputOutput,
                                win.vis->visual, CWBorderPixel|CWColormap|CWEventMask, &win.swa);
        if(!win.win) die("%s","[X11]: Failed to create window\n");
        XFree(win.vis);
        XStoreName(win.dpy, win.win, "desk");
        XMapWindow(win.dpy, win.win);
        win.wm_delete_window = XInternAtom(win.dpy, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(win.dpy, win.win, &win.wm_delete_window, 1);
    }
    {
        typedef GLXContext(*glxCreateContext)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
        int(*old_handler)(Display *, XErrorEvent *) = XSetErrorHandler(gl_error_handler);
        const char * extensions_str = glXQueryExtensionsString(win.dpy, DefaultScreen(win.dpy));
        glxCreateContext create_context = (glxCreateContext)
                                          glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
        gl_err = nk_false;
        if(!has_extension(extensions_str, "GLX_ARB_create_context") || !create_context)
        {
            fprintf(stderr, "[X11]: glXCreateContextAttribARB() not found...\n");
            fprintf(stderr, "[X11]: ... using old-style GLX context\n");
            glContext = glXCreateNewContext(win.dpy, win.fbc, GLX_RGBA_TYPE, 0, True);
        }
        else
        {
            GLint attr[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                None
            };
            glContext = create_context(win.dpy, win.fbc, 0, True, attr);
            XSync(win.dpy, False);
            if(gl_err || !glContext)
            {
                attr[1] = 1;
                attr[3] = 0;
                gl_err = nk_false;
                fprintf(stdout, "[X11] Failed to create OpenGL 3.0 context\n");
                fprintf(stdout, "[X11] ... using old-style GLX context!\n");
                glContext = create_context(win.dpy, win.fbc, 0, True, attr);
            }
        }
        XSync(win.dpy, False);
        XSetErrorHandler(old_handler);
        if(gl_err || !glContext)
            die("%s","[X11]: Failed to create an OpenGL context\n");
        glXMakeCurrent(win.dpy, win.win, glContext);
    }
    ctx = nk_x11_init(win.dpy, win.win);
    {
        struct nk_font_atlas * atlas;
        nk_x11_font_stash_begin(&atlas);
        nk_x11_font_stash_end();
    }
    iconidx=fileopentobuff(FAILSAFEICONIDX);
    openidx=fileopentobuff(FAILSAFEOPENIDX);
    hpath=getenv("HOME");
    desktop_dir.d_path =malloc(strlen(hpath)+strlen(folder)+1);
    strcpy(desktop_dir.d_path,hpath);
    strcat(desktop_dir.d_path,folder);
    desktop_dir.d_open = open(desktop_dir.d_path, O_RDONLY);
    EV_SET(&desktop_dir.d_change,desktop_dir.d_open, EVFILT_VNODE,
           EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR,
           NOTE_WRITE,
           0, 0);
    unsigned int fnum=0;
    desktop_dir.d=opendir(desktop_dir.d_path);
    if(desktop_dir.d)
    {
        while((dir=readdir(desktop_dir.d))!= NULL)
        {
            if(!(!strcmp(".",dir->d_name)||!strcmp("..",dir->d_name)))
            {
                fnum++;
            }
        }
    }
    else
    {
        die("%s\n","No desktop folder");
    }
    struct fileinfo ** files;
    printf("%d\n",fnum);
    files=malloc(fnum * sizeof(struct fileinfo *));
    size_t fi=0;
    rewinddir(desktop_dir.d);
    while((dir=readdir(desktop_dir.d))!= NULL)
    {
        if(!(!strcmp(".",dir->d_name)||!strcmp("..",dir->d_name)))
        {
            files[fi]=new_file(desktop_dir.d_path,dir->d_name,iconidx,magic_cookie_mime,magic_cookie_hr);
            fi++;
        }
    }
    puts("Loading icons");
    for(unsigned int i = 0; i<fnum ; i++)
    {
        printf("fis %d\n",i) ;
        start_thrd_for_icon(files,i,i);
    }
    struct sortby st;
    st.ac=1;
    st.st=NAME;
    sortfiles(files,fnum,st);
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.94f, bg.a = 1.0f;
    bgimage=load_image_open_resize(BGIMAGE, desktop_width,desktop_hight);
    struct menupos menupos;
    menupos.isactive=false;
    while(running)
    {
        XEvent evt;
        nk_input_begin(ctx);
        while(XPending(win.dpy))
        {
            XNextEvent(win.dpy, &evt);
            if(evt.type == ClientMessage) goto cleanup;
            if(XFilterEvent(&evt, win.win)) continue;
            nk_x11_handle_event(&evt);
        }
        nk_input_end(ctx);
        ctx->style.window.padding = nk_vec2(0,0);
        ctx->style.window.spacing = nk_vec2(0,0);
        ctx->style.window.scrollbar_size = nk_vec2(0,0);
        if(nk_begin(ctx, "desk", nk_rect(0, 0, desktop_width, desktop_hight),0))
        {
            files= updatefiles(desktop_dir,&fnum,kqid,files,iconidx,magic_cookie_mime,magic_cookie_hr);
            struct nk_window * win;
            win = ctx->current;
            nk_draw_image(&win->buffer,nk_rect(0,0,desktop_width,desktop_hight),&bgimage,nk_rgb(255,255,255));
            int row=0;
            unsigned int col=0;
            unsigned int maximum_cols=(desktop_width-(MINLFPAD+MINRIPAD))/(ICON_W+ICONHPAD);
            float calpad = (float)((desktop_width-(MINLFPAD+MINRIPAD)) %(ICON_W+ICONHPAD)) /2;
            nk_layout_space_begin(ctx, NK_STATIC, 70,maximum_cols);
            while(row<(fnum/maximum_cols)+1)
            {
                col=0;
                while((col+(row*maximum_cols))<fnum&&col<maximum_cols)
                {
                    int iconnum=col+(row*maximum_cols);
                    struct nk_rect icrect = nk_rect((col*(ICONHPAD+ICON_W)+MINLFPAD+calpad),(row* (ICON_W+20+50))+10, ICON_W, ICON_W+20);
                    if(files[iconnum]->icon_load_args.genid)
                    {
                        loadicon(files[iconnum]);
                    };
                    draw_icon(ctx,files[iconnum]->name,(files[iconnum]),openidx,icrect);
                    if(nk_input_is_mouse_click_in_rect(&ctx->input,NK_BUTTON_RIGHT,icrect))
                    {
                        if(nk_input_is_key_down((&ctx->input),NK_KEY_SHIFT))
                        {
                            files[iconnum]->isselected=!files[iconnum]->isselected;
                        }
                        else
                        {
                            menupos.isactive=true;
                            menupos.pos.x=*(&ctx->input.mouse.pos.x);
                            menupos.pos.y=*(&ctx->input.mouse.pos.y);
                        }
                    }
                    if(nk_input_is_mouse_click_in_rect(&ctx->input,NK_BUTTON_MIDDLE,icrect))
                    {
                        files[iconnum]->isselected=!files[iconnum]->isselected;
                    }
                    if(files[iconnum]->isselected)
                    {
                        printf("sel%d\n",col+(row*maximum_cols));
                        nk_stroke_rect(&win->buffer,icrect,0,2,nk_rgba(250,250,250,32));
                        nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,255,32));
                    }
                    col++;
                }
                row++;
            }
            if(nk_input_has_mouse_click(&ctx->input,0))
            {
                lastpos.x=*(&ctx->input.mouse.pos.x);
                lastpos.y=*(&ctx->input.mouse.pos.y);
            }
        }
        nk_end(ctx);
        if(nk_input_is_mouse_hovering_rect(&ctx->input,nk_rect(menupos.pos.x,menupos.pos.y, 100, 180)))
        {
            if(menupos.isactive)
            {
                if(nk_begin(ctx, "menu", nk_rect(menupos.pos.x,menupos.pos.y, 100, 180),0))
                {
                    nk_layout_row_dynamic(ctx,30,1);
                    if(nk_button_label(ctx,"delete"))
                    {
                        menupos.isactive=false;
                        int sif=0;
                        while(sif<fnum)
                        {
                            if(files[sif]->isselected==true)
                            {
                                remove(files[sif]->path);
                            }
                            files[sif]->isselected=false;
                            sif++;
                        }
                    }
                    nk_layout_row_dynamic(ctx,30,1);
                    nk_button_label(ctx,"rename");
                    nk_layout_row_dynamic(ctx,30,1);
                    nk_button_label(ctx,"move");
                    nk_layout_row_dynamic(ctx,30,1);
                    nk_button_label(ctx,"lunch");
                    nk_layout_row_dynamic(ctx,30,1);
                    nk_button_label(ctx,"open with");
                    nk_layout_row_dynamic(ctx,30,1);
                    nk_button_label(ctx,"details");
                    nk_end(ctx);
                }
            }
        }
        else
        {
            menupos.isactive=false;
        }
        XGetWindowAttributes(win.dpy, win.win, &win.attr);
        glViewport(0, 0, win.width, win.height);
        glClear(GL_COLOR_BUFFER_BIT);
        nk_x11_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glXSwapBuffers(win.dpy, win.win);
    }
cleanup:
    nk_x11_shutdown();
    glXMakeCurrent(win.dpy, 0, 0);
    glXDestroyContext(win.dpy, glContext);
    XUnmapWindow(win.dpy, win.win);
    XFreeColormap(win.dpy, win.cmap);
    XDestroyWindow(win.dpy, win.win);
    XCloseDisplay(win.dpy);
    return 0;
    return 0;
}
