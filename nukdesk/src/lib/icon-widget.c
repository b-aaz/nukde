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
#include <stdio.h>                    // for NULL, puts
#include <stdlib.h>                   // for malloc
#include <string.h>                   // for strcat, strlen, strcpy
#include <unistd.h>                   // for execl, fork

#include "../../../colibs/bool.h"   // for bool
#include "config-parser.h"            // for get_config
#include "fileinfo_type.h"            // for fileinfo, magicline

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_XLIB_LOAD_OPENGL_EXTENSIONS
#define NK_IMPLEMENTATION
#include "../../../colibs/nuklear.h"  // for nk_rect, nk_rgb, nk_rgba, nk_vec2

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#define NUERRCOLOR ""
#define NUERRCOLORRE ""
#include "../../../colibs/err.h"      // for die

bool input_mouse_has_clicked_even_times_in_rect(struct nk_input * in,enum nk_buttons id,struct nk_rect rect,bool * downup)
{
    if(nk_input_is_mouse_click_in_rect(in,id,rect))
    {
        *downup=!*downup;
    }
    return *downup;
}
int pos_is_in_rect(struct nk_vec2 v,struct nk_rect r)
{
    if(v.x>r.x&&v.x<r.x+r.w && v.y>r.y&&v.y<r.y+r.h)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void lunch(char * openbuff,struct fileinfo file)
{
    char * programname;
    char * lunchpath;
    programname = get_config(openbuff,file.magic.mime,file.magic.humanreadable,NULL);
    if(programname==NULL)
    {
        programname = get_config(openbuff,"default",NULL,NULL);
        if(programname==NULL)
        {
            die("%s\n","No programs found");
        }
    }
    lunchpath = malloc(strlen(programname)+1+strlen(file.path)+2);
    strcpy(lunchpath,programname);
    strcat(lunchpath," ");
    strcat(lunchpath,file.path);
    puts(lunchpath);
    if(fork()==0)
    {
        execl("/bin/sh","sh","-c",lunchpath,NULL);
    }
}
void draw_icon(struct nk_context * ctx,char * name,struct fileinfo * file,char * openbuff,struct nk_rect icrect)
{
    struct nk_window * win;
    struct nk_text text ;
    struct nk_input *in;
    win = ctx->current;
    in =  &ctx->input;
    text.padding =nk_vec2(0,0);
    text.background = nk_rgb(0,0,0);
    text.text = nk_rgb(250,250,250);
    if(nk_input_is_mouse_hovering_rect(in,icrect))
    {
        if(in->mouse.buttons[NK_BUTTON_LEFT].down)
        {
            nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,0,42));
            if(in->mouse.buttons[NK_BUTTON_LEFT].clicked)
            {
                lunch(openbuff, *file);
            }
        }
        /*The shadow around the icon when cursor is on it*/
        nk_stroke_rect(&win->buffer,icrect,0,2,nk_rgba(250,250,250,32));
        nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,0,32));
    }
    nk_widget_text(&win->buffer,icrect,name,nk_strlen(name),&text,NK_TEXT_ALIGN_CENTERED|NK_TEXT_ALIGN_BOTTOM,ctx->style.font);
    nk_draw_image(&win->buffer,nk_rect(icrect.x,icrect.y,icrect.w,icrect.w),file->return_image,nk_rgb(255,255,255));
}
