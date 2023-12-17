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
#include <stdio.h>                    /* for NULL, puts*/
#include <stdlib.h>                   /* for malloc*/
#include <string.h>                   /* for strcat, strlen, strcpy*/
#include <unistd.h>                   /* for execl, fork*/

#include "../../../colibs/bool.h"   /* for bool*/
#include "config-parser.h"            /* for get_config*/
#include "fileinfo_type.h"            /* for fileinfo, magicline*/

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
#include "../../../colibs/nuklear.h"  /* for nk_rect, nk_rgb, nk_rgba, nk_vec2*/
#include "nuklear_xlib_gl3.h"  

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#include "../../../colibs/err.h"      /* for die*/

static bool input_mouse_has_clicked_even_times_in_rect(struct nk_input * in,enum nk_buttons id,struct nk_rect rect,bool * downup)
{
    if(nk_input_is_mouse_click_in_rect(in,id,rect))
    {
        *downup=!*downup;
    }
    return *downup;
}
static int pos_is_in_rect(struct nk_vec2 v,struct nk_rect r)
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
void launch(char * openbuff,struct fileinfo file)
{
    char * programname;
    char * launchpath;
    programname = get_config(openbuff,file.type.mime,file.type.humanreadable,NULL);
    if(programname==NULL)
    {
        programname = get_config(openbuff,"default",NULL,NULL);
        if(programname==NULL)
        {
            die("%s\n","No programs found");
        }
    }
    launchpath = malloc(strlen(programname)+1+strlen(file.path)+2);
    strcpy(launchpath,programname);
    strcat(launchpath," ");
    strcat(launchpath,file.path);
    puts(launchpath);
    if(fork()==0)
    {
        execl("/bin/sh","sh","-c",launchpath,NULL);
    }
}
/* Draws a symbol indicating clipped text in the given rectangle rect. */
static void draw_cutoff_symbol(struct nk_command_buffer * cmd ,struct nk_rect rect ,struct nk_color color){
	/* Calculating the vertexes position . */
	
	/* The upper triangle . */	
	float utp1x = rect.x			;
	float utp1y = rect.y			;
	float utp2x = rect.x + rect.w		;
	float utp2y = rect.y + rect.h/4		;
	float utp3x = rect.x			;
	float utp3y = rect.y + rect.h/2		;
		
	/* The lower triangle . */	
	float ltp1x = rect.x			;
	float ltp1y = rect.y + rect.h/2		;
	float ltp2x = rect.x + rect.w		;
	float ltp2y = rect.y + rect.h/4 * 3	;
	float ltp3x = rect.x			;
	float ltp3y = rect.y + rect.h		;
	
	nk_fill_triangle(cmd,utp1x,utp1y,utp2x,utp2y,utp3x,utp3y,color);
	nk_fill_triangle(cmd,ltp1x,ltp1y,ltp2x,ltp2y,ltp3x,ltp3y,color);
}
void draw_icon(struct nk_context * ctx,char * name,struct fileinfo * file,char * openbuff,struct nk_rect icrect)
{
	struct nk_window * win;
	struct nk_text text ;
	struct nk_input *in;
	const struct nk_user_font * font;
	float namewidth;
	float fonth;
	struct nk_rect text_rect=icrect;
	win = ctx->current;
	in =  &ctx->input;
	font = ctx->style.font;
	fonth=font->height;
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
				launch(openbuff, *file);
			}
		}
		/* The shadow around the icon when cursor is on it . */
		nk_stroke_rect(&win->buffer,icrect,0,2,nk_rgba(250,250,250,32));
		nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,0,32));
	}

	/* Pixel width of the filename . */	
	namewidth=  font->width(font->userdata,fonth,name,nk_strlen(name));
	/* Draws a little symbol at the end of filename if it's too big to fit
	 * the icon . */	
	if (namewidth > icrect.w)
	{
		float symbolw = font->width(font->userdata,fonth,"_",1);
		struct nk_rect symbol = nk_rect(
				  icrect.x + icrect.w - symbolw,
				  icrect.y + icrect.h - fonth,
				symbolw,
				  fonth
				);
		
		text_rect=nk_rect(icrect.x,icrect.y,icrect.w-symbolw*2,icrect.h);
		draw_cutoff_symbol(&win->buffer,symbol,text.text);
	}
	nk_widget_text(&win->buffer,text_rect,name,nk_strlen(name),&text,NK_TEXT_ALIGN_CENTERED|NK_TEXT_ALIGN_BOTTOM,font);
	if (file->return_image->handle.id)
	{
		nk_draw_image(&win->buffer,nk_rect(icrect.x,icrect.y,icrect.w,icrect.w),file->return_image,nk_rgb(255,255,255));
	}
}
