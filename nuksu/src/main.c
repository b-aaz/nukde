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
#include <malloc_np.h>
#include <X11/cursorfont.h>
#include "./lib/strrmadd.h"
#include "./lib/auth.h"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION
#define INCLUDE_STYLE
/*#define NK_INPUT_MAX 2 */
#include "../../colibs/nuklear.h"
#include "../../colibs/bool.h"
#include "./lib/nuklear_xlib.h"
#define DTIME          20
#define CHAR_REPEAT_DELAY 100
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
enum { NS_PER_SECOND = 1000000000 };
void sub_timespec(struct timespec t1, struct timespec t2,
                  struct timespec * td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if(td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if(td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}
static long
timestamp(void)
{
    struct timeval tv;
    if(gettimeofday(&tv, NULL) < 0) return 0;
    return (long)((long) tv.tv_sec * 1000 + (long) tv.tv_usec / 1000);
}
static void
sleep_for(long t)
{
    struct timespec req;
    const time_t sec = (int)(t / 1000);
    const long ms = t - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = ms * 1000000L;
    while(-1 == nanosleep(&req, &req));
}
struct pass
{
    char * buf;
    size_t bufsize;
    size_t length;
};














struct lock_style
{
    short int lock_speed;

    struct nk_color shackle;
    struct nk_color body;
    struct nk_color key_hole;
};

void draw_lock(struct nk_command_buffer * b,struct nk_rect lockpos,int openperc,struct lock_style lockstyle)
{
    struct nk_rect lock;
    float i1m=0, i1=0,i2m=0, i2=0;
    float perc2i=0;
    lock.h=(lockpos.h-lockpos.h/21)/2;
    lock.w=(lockpos.w-lockpos.w/21)/2;
    lock.y=lockpos.y+lock.h;
    lock.x=lockpos.x+lock.w-(lock.w/5*2);
    i1m=lock.h/5;
    i2m=(lock.w-lock.w/5*2)*2;
    perc2i=openperc*((i1m+i2m)/100);
    if(perc2i >= i1m)
    {
        i1=i1m;
        i2=perc2i-i1m;
    }
    else
    {
        i1=perc2i;
    }
    nk_stroke_line(b,lock.x+lock.w/5,lock.y-lock.h/5-i1,lock.x+lock.w/5,lock.y+lock.h/5-i1,lock.w/6.25,lockstyle.shackle);
    nk_stroke_line(b,lock.x+lock.w/5*4-i2,lock.y-lock.h/5-i1,lock.x+lock.w/5*4-i2,lock.y-i1,lock.w/6.25,lockstyle.shackle);
    nk_stroke_curve(b, lock.x+lock.w/5, lock.y-lock.h/5-i1,lock.x+lock.w/5, lock.y-lock.h*0.7-i1,lock.x+lock.w/5*4-i2,lock.y-lock.h*0.7-i1, lock.x+lock.w/5*4-i2, lock.y-lock.h/5-i1, lock.w/6.25, lockstyle.shackle);
    nk_fill_rect(b, lock, lock.w/6.25, lockstyle.body);
    nk_stroke_rect(b,lock,lock.w/6.25,lock.h/100,lockstyle.body);
    nk_fill_circle(b, nk_rect(lock.x+lock.w/2-lock.w/10, lock.y+lock.h/2-lock.h/10, lock.w/5,lock.h/5), lockstyle.key_hole);
    nk_fill_triangle(b, lock.x+lock.w/2, lock.y+lock.h/2-lock.h/10, lock.x+lock.w/5*3,lock.y+lock.h/5*4, lock.x+lock.w/5*2, lock.y+lock.h/5*4, lockstyle.key_hole);
}



void input_mouse_has_clicked_even_times_in_rect(struct nk_input * in,enum nk_buttons id,struct nk_rect rect,bool * downup)
{
    if(nk_input_is_mouse_click_in_rect(in,id,rect))
    {
        *downup=!*downup;
    }
}






void lock_icon(struct nk_context * ctx,struct nk_rect lockpos,short int speed,int * lockopeness,bool * downup,struct lock_style lockstyle)
{
    struct nk_input * in;
    struct nk_command_buffer * b;
    in = &ctx->input;
    b = &ctx->current->buffer;
    input_mouse_has_clicked_even_times_in_rect(in,NK_BUTTON_LEFT,lockpos,downup);
    if(*downup==0)
    {
        if(*lockopeness>0)
        {
            *lockopeness-=speed;
        }
    }
    else
    {
        if(*lockopeness<100)
        {
            *lockopeness+=speed;
        }
    }
    if(*lockopeness>100)
    {
        *lockopeness=100;
    }
    if(*lockopeness<0)
    {
        *lockopeness=0;
    }
    draw_lock(b,lockpos,*lockopeness,lockstyle);
}









































struct passwords_input_data
{


    bool active ;
    size_t cursor_pos;
    size_t shift;
    bool showpass ;
    long lastt;
    int lockopeness;
};

struct passwords_input_style
{
    struct lock_style lockstyle;
    struct nk_color cursor_color;
    struct nk_color background_color;
    struct nk_color label_textcolor;
    struct nk_color label_backgroundcolor;
    struct nk_color pass_textcolor;
    struct nk_color pass_backgroundcolor;
    struct nk_color fild_activecolor;
    struct nk_color fild_inactivecolor;
    short unsigned int space_betwen_charecters;
    short unsigned int text_arias_start_padding;
    char * label;
};
void password_input(struct nk_context * ctx,struct passwords_input_data * stuff,struct passwords_input_style ipstyle,struct pass * pass,XWindow xw,Cursor cu)
{
    struct nk_window * win;
    struct nk_style * style;
    size_t s;
    long thist;
    float row_height;
    struct nk_input * in;
    const struct nk_style_edit * editstyle;
    struct nk_rect widpos;
    struct nk_rect text_aria;
    short unsigned int wids_end_buff_space;
    size_t maxchn;
    struct nk_rect label_aria;
    struct nk_rect lockspos;
    int shiftvalue;
    nk_widget(&widpos, ctx);
    in =  &ctx->input;
    win = ctx->current;
    style = &ctx->style;
    editstyle = &style->edit;
    row_height = style->font->height;
    wids_end_buff_space=widpos.h;
    maxchn = (int)(((widpos.w - wids_end_buff_space-ipstyle.text_arias_start_padding) /ipstyle.space_betwen_charecters));
    text_aria.x = widpos.x + ipstyle.text_arias_start_padding;
    text_aria.y = widpos.y + widpos.h/2-row_height/2;
    text_aria.h = row_height;
    shiftvalue=1;
    text_aria.w = (pass->length - stuff->shift)*ipstyle.space_betwen_charecters;
    if(text_aria.w>maxchn*ipstyle.space_betwen_charecters)
    {
        text_aria.w=maxchn*ipstyle.space_betwen_charecters;
    }
    if(nk_input_is_mouse_hovering_rect(in,widpos))
    {
        XDefineCursor(xw.dpy,xw.win,cu);
    }
    else
    {
        XUndefineCursor(xw.dpy,xw.win);
    }
    if(nk_input_has_mouse_click(in, NK_BUTTON_LEFT))
    {
        if(in->mouse.pos.y > widpos.y
                && in->mouse.pos.y < widpos.y + widpos.h
                && in->mouse.pos.x > text_aria.x + text_aria.w
                && in->mouse.pos.x < widpos.x + widpos.w-wids_end_buff_space)
        {
            stuff->cursor_pos=(pass->length-stuff->shift);
        }
        if(in->mouse.pos.y > widpos.y
                && in->mouse.pos.y < widpos.y + widpos.h
                && in->mouse.pos.x > text_aria.x
                && in->mouse.pos.x < text_aria.x + text_aria.w)
        {
            stuff->cursor_pos = text_aria.x;
            for(; in->mouse.pos.x > stuff->cursor_pos; stuff->cursor_pos += ipstyle.space_betwen_charecters)
            {
            }
            stuff->cursor_pos -= text_aria.x;
            stuff->cursor_pos /= ipstyle.space_betwen_charecters;
            stuff->cursor_pos--;
        }
        if(!nk_input_is_mouse_hovering_rect(in, widpos))
        {
            stuff->active = 0;
        }
        else
        {
            stuff->active = 1;
        }
    }
    nk_fill_rect(&win->buffer, widpos, editstyle->rounding,ipstyle.background_color);
    if(stuff->active)
    {
        nk_stroke_rect(&win->buffer, widpos,editstyle->rounding,editstyle->border,ipstyle.fild_activecolor);
        if(nk_input_is_key_down(in,NK_KEY_DEL)&&stuff->cursor_pos+stuff->shift<pass->length)
        {
            thist=timestamp();
            if(thist-stuff->lastt>=CHAR_REPEAT_DELAY)
            {
                strrm(pass->buf,pass->length, stuff->cursor_pos+stuff->shift, 1);
                pass->length--;
                stuff->lastt=thist;
            }
        }
        if(nk_input_is_key_down(in,NK_KEY_RIGHT)&&stuff->cursor_pos+stuff->shift<pass->length)
        {
            thist=timestamp();
            if(thist-stuff->lastt>=CHAR_REPEAT_DELAY)
            {
                stuff->cursor_pos++;
                if(stuff->cursor_pos>maxchn)
                {
                    stuff->shift+=shiftvalue;
                    stuff->cursor_pos-=shiftvalue;
                }
                stuff->lastt=thist;
            }
        }
        if(nk_input_is_key_down(in, NK_KEY_LEFT))
        {
            thist=timestamp();
            if(thist-stuff->lastt>=CHAR_REPEAT_DELAY && (stuff->cursor_pos||stuff->shift))
            {
                if(stuff->shift!=0&&stuff->cursor_pos==0)
                {
                    stuff->shift-=shiftvalue;
                    stuff->cursor_pos+=shiftvalue;
                }
                stuff->cursor_pos--;
                stuff->lastt=thist;
            }
        }
        if(nk_input_is_key_down(in, NK_KEY_BACKSPACE)&&(stuff->cursor_pos||stuff->shift))
        {
            thist=timestamp();
            if(thist-stuff->lastt>=CHAR_REPEAT_DELAY)
            {
                strrm(pass->buf,pass->length, stuff->cursor_pos - 1+stuff->shift, 1);
                if(stuff->shift)
                {
                    stuff->shift-=shiftvalue;
                }
                else
                {
                    stuff->cursor_pos--;
                }
                pass->length--;
                stuff->lastt=thist;
                if(stuff->shift!=0&&stuff->cursor_pos==0)
                {
                    stuff->shift-=shiftvalue;
                    stuff->cursor_pos+=shiftvalue;
                }
            }
        }
        if(in->keyboard.text_len)
        {
            if(pass->length >= pass->bufsize)
            {
                pass->buf = realloc(pass->buf, (pass->length + 32));
                pass->bufsize =+32;
            }
            stradd(pass->buf, pass->length, in->keyboard.text,in->keyboard.text_len, stuff->cursor_pos+stuff->shift);
            stuff->cursor_pos+=in->keyboard.text_len;
            pass->length+=in->keyboard.text_len;
            if(stuff->cursor_pos>maxchn-1)
            {
                stuff->cursor_pos-=in->keyboard.text_len;
                stuff->shift+=in->keyboard.text_len;
            }
        }
        nk_edit_draw_text(&win->buffer, &style->edit,text_aria.x+ (stuff->cursor_pos * ipstyle.space_betwen_charecters),text_aria.y,
                          0," ", 1, row_height, style->font,ipstyle.cursor_color,ipstyle.cursor_color, nk_true);
    }
    else
    {
        nk_stroke_rect(&win->buffer, widpos, editstyle->rounding, editstyle->border,ipstyle.fild_inactivecolor);
    }
    if(stuff->showpass)
    {
        /*show*/
        for(s = 0; ((s < (pass->length-stuff->shift))  && (s < maxchn)); s++)
        {
            if(s == stuff->cursor_pos&&stuff->active)
            {
                nk_edit_draw_text(&win->buffer, &style->edit, text_aria.x+(s*ipstyle.space_betwen_charecters),text_aria.y,
                                  0, &pass->buf[s+stuff->shift], 1, row_height, style->font, ipstyle.pass_textcolor, ipstyle.pass_backgroundcolor, nk_true);
            }
            else
            {
                nk_edit_draw_text(&win->buffer, &style->edit,text_aria.x+(s*ipstyle.space_betwen_charecters),text_aria.y,
                                  0, &pass->buf[s+stuff->shift], 1, row_height, style->font, ipstyle.pass_backgroundcolor, ipstyle.pass_textcolor, nk_false);
            }
        }
    }
    else
    {
        /*hide*/
        for(s = 0; (s < (pass->length-stuff->shift) && (s < maxchn)); s++)
        {
            if(s == stuff->cursor_pos&&stuff->active)
            {
                nk_edit_draw_text(&win->buffer, &style->edit,text_aria.x+(s*ipstyle.space_betwen_charecters),text_aria.y,
                                  0, "*", 1, row_height, style->font,ipstyle.pass_textcolor,ipstyle.pass_backgroundcolor, nk_true);
            }
            else
            {
                nk_edit_draw_text(&win->buffer, &style->edit,text_aria.x+(s*ipstyle.space_betwen_charecters),text_aria.y,
                                  0, "*", 1, row_height, style->font, ipstyle.pass_backgroundcolor, ipstyle.pass_textcolor, nk_false);
            }
        }
    }
    if(!pass->length)
    {
        label_aria.y=text_aria.y;
        label_aria.x=text_aria.x+ipstyle.space_betwen_charecters;
        label_aria.w=ipstyle.space_betwen_charecters*maxchn;
        label_aria.h=row_height;
        nk_draw_text(&win->buffer,label_aria,ipstyle.label+1,nk_strlen(ipstyle.label)-1,style->font,ipstyle.label_backgroundcolor,ipstyle.label_textcolor);
        if(stuff->active)
        {
            nk_edit_draw_text(&win->buffer, &style->edit, text_aria.x,text_aria.y, 0,&ipstyle.label[0],
                              1, row_height,style->font,ipstyle.cursor_color,ipstyle.label_textcolor, nk_true);
        }
        else
        {
            nk_edit_draw_text(&win->buffer, &style->edit, text_aria.x,text_aria.y, 0,&ipstyle.label[0],
                              1, row_height,style->font,ipstyle.label_backgroundcolor,ipstyle.label_textcolor, nk_false);
        }
    }
    lockspos=nk_rect(widpos.x+widpos.w-widpos.h,widpos.y,widpos.h,widpos.h);
    lock_icon(ctx,lockspos,ipstyle.lockstyle.lock_speed,&stuff->lockopeness,&stuff->showpass,ipstyle.lockstyle);
    /*nk_fill_rect(&win->buffer,text_aria,0,nk_rgb(255,0,0));*/
}















































int main(int argc,char * argv[])
{
    long dt;
    long started;
    struct nk_context * ctx;
    bool running = 1;
    bool warn=0;
    time_t utime=0;
    short int tries=0;
    XWindow xw;
    struct pass pass;
    FILE * init;
    struct passwords_input_data pd ;
    pd.active=0;
    pd.lockopeness=0;
    pd.lastt=0;
    pd.shift=0;
    pd.cursor_pos=0;
    pd.showpass=0;
    if(argc==1)
    {
        die("%s\n","No programs specified ");
    }
    if(access(WARNFILE_LOCATION,F_OK))
    {
        init=fopen(WARNFILE_LOCATION,"w");
        if(init==NULL)
        {
            die("%s\n","Can not open the warning file");
        }
        fclose(init);
    }
    pass.bufsize = 2;
    pass.length=0;
    pass.buf = malloc(pass.bufsize * sizeof(char));
    memset(&xw, 0, sizeof xw);
    xw.dpy = XOpenDisplay(NULL);
    if(!xw.dpy)
        die("%s\n","Could not open a display; perhaps $DISPLAY is not set?");
    xw.root = DefaultRootWindow(xw.dpy);
    xw.screen = XDefaultScreen(xw.dpy);
    xw.vis = XDefaultVisual(xw.dpy, xw.screen);
    xw.cmap = XCreateColormap(xw.dpy, xw.root, xw.vis, AllocNone);
    xw.swa.colormap = xw.cmap;
    xw.swa.event_mask =
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask | ButtonMotionMask |
        Button1MotionMask | Button3MotionMask | Button4MotionMask |
        Button5MotionMask |
        PointerMotionMask | KeymapStateMask;
    xw.win = XCreateWindow(xw.dpy, xw.root, 0, 0, WINDOW_WIDTH,
                           WINDOW_HEIGHT, 0,
                           XDefaultDepth(xw.dpy, xw.screen), InputOutput,
                           xw.vis, CWEventMask | CWColormap, &xw.swa);
    XStoreName(xw.dpy, xw.win, "X11");
    XMapWindow(xw.dpy, xw.win);
    xw.wm_delete_window = XInternAtom(xw.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(xw.dpy, xw.win, &xw.wm_delete_window, 1);
    XGetWindowAttributes(xw.dpy, xw.win, &xw.attr);
    xw.width = (unsigned int) xw.attr.width;
    xw.height = (unsigned int) xw.attr.height;
    xw.font = nk_xfont_create(xw.dpy, "fixed");
    ctx = nk_xlib_init(xw.font, xw.dpy, xw.screen, xw.win, xw.width,
                       xw.height);
#ifdef INCLUDE_STYLE
    set_style(ctx, THEME_RED);
#endif
    while(running)
    {
        XEvent evt;
        started = timestamp();
        nk_input_begin(ctx);
        while(XPending(xw.dpy))
        {
            XNextEvent(xw.dpy, &evt);
            if(evt.type == ClientMessage) goto cleanup;
            if(XFilterEvent(&evt, xw.win)) continue;
            nk_xlib_handle_event(xw.dpy, xw.screen, xw.win, &evt);
        }
        nk_input_end(ctx);
        XGetWindowAttributes(xw.dpy, xw.win, &xw.attr);
        xw.width = (unsigned int) xw.attr.width;
        xw.height = (unsigned int) xw.attr.height;
        if(nk_begin(ctx, "Demo", nk_rect(0, 0,xw.width,xw.height),0))
        {
            struct nk_window * win;
            struct nk_style * style;
            struct nk_rect warnwidpos;
            time_t timenow;
            struct nk_text warningtext;
            struct passwords_input_style  pds;
            char warning [100];
            FILE * warnfile;
            Cursor cu;
            //warning
            cu = XCreateFontCursor(xw.dpy,XC_xterm);
            win = ctx->current;
            style = &ctx->style;
            warningtext.text =nk_rgb(0,0,0);
            warningtext.background =nk_rgb(255,0,0);
            nk_layout_row_dynamic(ctx, 30, 1);
            pds.cursor_color=nk_rgb(255,255,255);
            pds.background_color=nk_rgb(68,71,90);
            pds.fild_inactivecolor=nk_rgb(200, 150, 100);
            pds.fild_activecolor=nk_rgb(100, 150, 200);
            pds.pass_textcolor=nk_rgb(255,255,255);
            pds.pass_backgroundcolor=nk_rgb(68,71,90);
            pds.label_textcolor=nk_rgb(0,0,0);
            pds.label_backgroundcolor=nk_rgb(68,71,90);
            pds.text_arias_start_padding=7;
            pds.space_betwen_charecters=6;
            pds.lockstyle.lock_speed=10;
            pds.lockstyle.shackle=nk_rgb(255,215,0);
            pds.lockstyle.body=nk_rgb(0,0,0);
            pds.lockstyle.key_hole=nk_rgb(255,255,255);
            pds.label="Enter pass";
            password_input(ctx,&pd,pds,&pass,xw,cu);
            nk_layout_row_dynamic(ctx, 25, 1);
            time(&timenow);
            if(nk_button_label(ctx, "Go!"))
            {
                warnfile=fopen(WARNFILE_LOCATION,"r");
                if(warnfile==NULL)
                {
                    die("%s\n","Can not open the warning file");
                }
                fscanf(warnfile,"%hd\n%ld",&tries,&utime);
                //printf("before\n%hd\n%ld\n",tries,utime);
                fclose(warnfile);
                if((timenow-utime)>DELAY)
                {
                    //fprintf(stdout, "button pressed\n");
                    if(auth(pass.buf,"/etc/master.passwd"))
                    {
                        execvp(argv[1],argv+1);
                        die("%s\n","Can not execute the program");
                    }
                    else
                    {
                        puts(WARN_COLOR"Wrong password"RESTC);
                        warn=1;
                        //printf("%d" ,warn);
                        tries++;
                        if(tries>=CHANCES)
                        {
                            tries=0;
                            utime=timenow;
                        }
                        warnfile=fopen(WARNFILE_LOCATION,"w");
                        if(warnfile==NULL)
                        {
                            die("%s\n","Can not open the warning file");
                        }
                        fprintf(warnfile,"%d\n%ld",tries,utime);
                        //printf("after\n%d\n%ld\n",tries,utime);
                        fclose(warnfile);
                    }
                }
                else
                {
                    warn=1;
                }
            }
            if(warn)
            {
                if(CHANCES-tries==CHANCES&&timenow-utime<DELAY)
                {
                    snprintf(warning,100,"Too meany tries wait for %lds",DELAY+utime-timenow);
                }
                else
                {
                    snprintf(warning,100,"Wrong password %d more tries",CHANCES-tries);
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_widget(&warnwidpos,ctx);
                    nk_fill_rect(&win->buffer, warnwidpos,5, nk_rgb(255,0,0));
                    nk_widget_text(&win->buffer,warnwidpos,warning,nk_strlen(warning),&warningtext,NK_TEXT_CENTERED,style->font);
                }
            }
        }
        nk_end(ctx);
        XClearWindow(xw.dpy, xw.win);
        nk_xlib_render(xw.win, nk_rgba(20, 30, 30, 0));
        XFlush(xw.dpy);
        dt = timestamp() - started;
        if(dt < DTIME)
            sleep_for(DTIME - dt);
    }
cleanup:
    free(pass.buf);
    nk_xfont_del(xw.dpy, xw.font);
    nk_xlib_shutdown();
    XUnmapWindow(xw.dpy, xw.win);
    XFreeColormap(xw.dpy, xw.cmap);
    XDestroyWindow(xw.dpy, xw.win);
    XCloseDisplay(xw.dpy);
    return 0;
}
