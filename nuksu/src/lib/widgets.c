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


#include <stdio.h>                         // for size_t, NULL
#include <stdlib.h>                        // for realloc
#include <sys/time.h>                      // for gettimeofday, timeval

#include "../../../colibs/bool.h"          // for bool, false, true

#define NK_INCLUDE_FIXED_TYPES
#define NK_IMPLEMENTATION
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#include "../../../colibs/nuklear.h"       // for nk_rect, nk_input, nk_edit...
#include "../../../colibs/nuklear_xlib.h"  // for XWindow
#include "strrmadd.h"                      // for strrm, stradd
#include "widgets.h"

#define CHAR_REPEAT_DELAY 100


static long timestamp (void)
{
	struct timeval tv;

	if (gettimeofday (&tv, NULL) < 0)
	{
		return 0;
	}

	return (long) ( (long) tv.tv_sec * 1000 + (long) tv.tv_usec / 1000);
}

static void draw_lock (struct nk_command_buffer * b,struct nk_rect lockpos,int openperc,struct lock_style lockstyle)
{
	struct nk_rect lock;
	float i1m=0, i1=0,i2m=0, i2=0;
	float perc2i=0;
	lock.h= (lockpos.h-lockpos.h/21) /2;
	lock.w= (lockpos.w-lockpos.w/21) /2;
	lock.y=lockpos.y+lock.h;
	lock.x=lockpos.x+lock.w- (lock.w/5*2);
	i1m=lock.h/5;
	i2m= (lock.w-lock.w/5*2) *2;
	perc2i=openperc* ( (i1m+i2m) /100);

	if (perc2i >= i1m)
	{
		i1=i1m;
		i2=perc2i-i1m;
	}
	else
	{
		i1=perc2i;
	}

	nk_stroke_line (b,
			lock.x+lock.w/5,
			lock.y-lock.h/5-i1,
			lock.x+lock.w/5,
			lock.y+lock.h/5-i1,
			lock.w/6.25,
			lockstyle.shackle);
	nk_stroke_line (b,
			lock.x+lock.w/5*4-i2,
			lock.y-lock.h/5-i1,
			lock.x+lock.w/5*4-i2,
			lock.y-i1,
			lock.w/6.25,
			lockstyle.shackle);
	nk_stroke_curve (b,
			lock.x+lock.w/5,
			lock.y-lock.h/5-i1,
			lock.x+lock.w/5,
			lock.y-lock.h*0.7-i1,
			lock.x+lock.w/5*4-i2,
			lock.y-lock.h*0.7-i1,
			lock.x+lock.w/5*4-i2,
			lock.y-lock.h/5-i1,
			lock.w/6.25,
			lockstyle.shackle);
	nk_fill_rect (b,
			lock,
			lock.w/6.25,
			lockstyle.body);
	nk_stroke_rect (b,
			lock,
			lock.w/6.25,
			lock.h/100,
			lockstyle.body);
	nk_fill_circle (b,
			nk_rect (lock.x+lock.w/2-lock.w/10,
				lock.y+lock.h/2-lock.h/10,
				lock.w/5,
				lock.h/5),
			lockstyle.key_hole);
	nk_fill_triangle (b,
			lock.x+lock.w/2,
			lock.y+lock.h/2-lock.h/10,
			lock.x+lock.w/5*3,
			lock.y+lock.h/5*4,
			lock.x+lock.w/5*2,
			lock.y+lock.h/5*4,
			lockstyle.key_hole);
}
static void input_mouse_has_clicked_even_times_in_rect (struct nk_input * in,enum nk_buttons id,struct nk_rect rect,bool * downup)
{
	if (nk_input_is_mouse_click_in_rect (in,id,rect))
	{
		*downup=!*downup;
	}
}

static void lock_icon (struct nk_context * ctx,struct nk_rect lockpos,short int speed,int * lockopeness,bool * downup,struct lock_style lockstyle)
{
	struct nk_input * in;
	struct nk_command_buffer * b;
	in = &ctx->input;
	b = &ctx->current->buffer;
	input_mouse_has_clicked_even_times_in_rect (in,NK_BUTTON_LEFT,lockpos,downup);

	if (*downup==0)
	{
		if (*lockopeness>0)
		{
			*lockopeness-=speed;
		}
	}
	else
	{
		if (*lockopeness<100)
		{
			*lockopeness+=speed;
		}
	}

	if (*lockopeness>100)
	{
		*lockopeness=100;
	}

	if (*lockopeness<0)
	{
		*lockopeness=0;
	}

	draw_lock (b,lockpos,*lockopeness,lockstyle);
}
void password_input (struct nk_context * ctx,struct passwords_input_data * data,struct passwords_input_style ipstyle,struct password * password,struct XWindow xw,Cursor cu)
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
	nk_widget (&widpos, ctx);
	in =  &ctx->input;
	win = ctx->current;
	style = &ctx->style;
	editstyle = &style->edit;
	row_height = style->font->height;
	wids_end_buff_space=widpos.h;

	maxchn =
		(int)
		(
		 (widpos.w - wids_end_buff_space-ipstyle.text_arias_start_padding)
		 /ipstyle.space_betwen_charecters
		);

	text_aria.x = widpos.x + ipstyle.text_arias_start_padding;
	text_aria.y = widpos.y + widpos.h/2-row_height/2;
	text_aria.h = row_height;
	shiftvalue=1;
	text_aria.w = (password->length - data->shift) *ipstyle.space_betwen_charecters;

	if (text_aria.w>maxchn*ipstyle.space_betwen_charecters)
	{
		text_aria.w=maxchn*ipstyle.space_betwen_charecters;
	}

	/* Changing the X cursor from "I beam" to default when we enter the
	 * password field and vice versa .
	 */
	if (nk_input_is_mouse_hovering_rect (in,widpos))
	{
		XDefineCursor (xw.dpy,xw.win,cu);
	}
	else
	{
		XUndefineCursor (xw.dpy,xw.win);
	}

	if (nk_input_has_mouse_click (in, NK_BUTTON_LEFT))
	{
		if (NK_INBOX (
					in->mouse.pos.x,
					in->mouse.pos.y,
					text_aria.x+text_aria.w,
					widpos.y,
					widpos.w-wids_end_buff_space,
					widpos.h
			     ))
		{
			data->cursor_pos= (password->length-data->shift);
		}

		if (NK_INBOX (
					in->mouse.pos.x,
					in->mouse.pos.y,
					text_aria.x,
					widpos.y,
					text_aria.w,
					widpos.h
			     ))
		{
			data->cursor_pos = text_aria.x;

			for (; in->mouse.pos.x > data->cursor_pos; data->cursor_pos += ipstyle.space_betwen_charecters)
			{
			}

			data->cursor_pos -= text_aria.x;
			data->cursor_pos /= ipstyle.space_betwen_charecters;
			data->cursor_pos--;
		}

		if (!nk_input_is_mouse_hovering_rect (in, widpos))
		{
			data->active = false;
		}
		else
		{
			data->active = true;
		}
	}

	nk_fill_rect (&win->buffer, widpos, editstyle->rounding,ipstyle.background_color);

	if (data->active)
	{
		nk_stroke_rect (&win->buffer, widpos,editstyle->rounding,editstyle->border,ipstyle.fild_activecolor);

		if (nk_input_is_key_down (in,NK_KEY_DEL) &&data->cursor_pos+data->shift<password->length)
		{
			thist=timestamp();

			if (thist-data->lastt>=CHAR_REPEAT_DELAY)
			{
				strrm (password->buf,password->length, data->cursor_pos+data->shift, 1);
				password->length--;
				data->lastt=thist;
			}
		}

		if (nk_input_is_key_down (in,NK_KEY_RIGHT) &&data->cursor_pos+data->shift<password->length)
		{
			thist=timestamp();

			if (thist-data->lastt>=CHAR_REPEAT_DELAY)
			{
				data->cursor_pos++;

				if (data->cursor_pos>maxchn)
				{
					data->shift+=shiftvalue;
					data->cursor_pos-=shiftvalue;
				}

				data->lastt=thist;
			}
		}

		if (nk_input_is_key_down (in, NK_KEY_LEFT))
		{
			thist=timestamp();

			if (thist-data->lastt>=CHAR_REPEAT_DELAY && (data->cursor_pos||data->shift))
			{
				if (data->shift!=0&&data->cursor_pos==0)
				{
					data->shift-=shiftvalue;
					data->cursor_pos+=shiftvalue;
				}

				data->cursor_pos--;
				data->lastt=thist;
			}
		}

		if (nk_input_is_key_down (in, NK_KEY_BACKSPACE) && (data->cursor_pos||data->shift))
		{
			thist=timestamp();

			if (thist-data->lastt>=CHAR_REPEAT_DELAY)
			{
				strrm (password->buf,password->length, data->cursor_pos - 1+data->shift, 1);

				if (data->shift)
				{
					data->shift-=shiftvalue;
				}
				else
				{
					data->cursor_pos--;
				}

				password->length--;
				data->lastt=thist;

				if (data->shift!=0&&data->cursor_pos==0)
				{
					data->shift-=shiftvalue;
					data->cursor_pos+=shiftvalue;
				}
			}
		}

		if (in->keyboard.text_len)
		{
			if (password->length >= password->bufsize)
			{
				password->buf = realloc (password->buf, (password->length + 32));
				password->bufsize =+32;
			}

			stradd (password->buf, password->length, in->keyboard.text,in->keyboard.text_len, data->cursor_pos+data->shift);
			data->cursor_pos+=in->keyboard.text_len;
			password->length+=in->keyboard.text_len;

			if (data->cursor_pos>maxchn-1)
			{
				data->cursor_pos-=in->keyboard.text_len;
				data->shift+=in->keyboard.text_len;
			}
		}

		nk_edit_draw_text (&win->buffer, &style->edit,text_aria.x+ (data->cursor_pos * ipstyle.space_betwen_charecters),text_aria.y,
				0," ", 1, row_height, style->font,ipstyle.cursor_color,ipstyle.cursor_color, nk_true);
	}
	else
	{
		nk_stroke_rect (&win->buffer, widpos, editstyle->rounding, editstyle->border,ipstyle.fild_inactivecolor);
	}

	for (s = 0; ( (s < (password->length-data->shift))  && (s < maxchn)); s++)
	{
		nk_edit_draw_text
			( &win->buffer ,&style->edit ,text_aria.x+ (s*ipstyle.space_betwen_charecters) ,text_aria.y	,0	,data->showpassword?&password->buf[s+data->shift]:"*" ,1		,row_height	,style->font
			  ,s==data->cursor_pos && data->active	?
			  ipstyle.pass_textcolor
			  :
			  ipstyle.pass_backgroundcolor
			  ,s==data->cursor_pos && data->active	?
			  ipstyle.pass_backgroundcolor
			  :
			  ipstyle.pass_textcolor
			  ,nk_true
			);
	}

	/* This is the part that shows a label in the input field if its empty . */
	if (!password->length)
	{
		label_aria.y=text_aria.y;
		label_aria.x=text_aria.x+ipstyle.space_betwen_charecters;
		label_aria.w=ipstyle.space_betwen_charecters*maxchn;
		label_aria.h=row_height;
		nk_draw_text (
				&win->buffer,
				label_aria,
				ipstyle.label+1,
				nk_strlen (ipstyle.label)-1,
				style->font,
				ipstyle.label_backgroundcolor,
				ipstyle.label_textcolor
			     );

		if (data->active)
		{
			nk_edit_draw_text (
					&win->buffer,
					&style->edit,
					text_aria.x,
					text_aria.y,
					0,
					ipstyle.label,
					1,
					row_height,
					style->font,
					ipstyle.cursor_color,
					ipstyle.label_textcolor,
					nk_true
					);
		}
		else
		{
			nk_edit_draw_text (
					&win->buffer,
					&style->edit,
					text_aria.x,
					text_aria.y,
					0,
					ipstyle.label,
					1,
					row_height,
					style->font,
					ipstyle.label_backgroundcolor,
					ipstyle.label_textcolor,
					nk_false
					);
		}
	}

	/* Showing the little interactive lock icon at the right side of the input 
	 * field . 
	 */
	lockspos=nk_rect (widpos.x+widpos.w-widpos.h,widpos.y,widpos.h,widpos.h);
	lock_icon (ctx,lockspos,ipstyle.lockstyle.lock_speed,&data->lockopeness,&data->showpassword,ipstyle.lockstyle);
}
void warning_widget ( struct nk_context* ctx, struct warning_style ws, char * warning) {


	struct nk_window * win;
	struct nk_rect warnwidpos;
	struct nk_text warningtext;
	win = ctx->current;
	warningtext.text =ws.text_forground;
	warningtext.background =ws.text_background;
	warningtext.padding =nk_vec2(0,0);
	nk_widget (&warnwidpos,ctx);
	nk_fill_rect (&win->buffer, warnwidpos,ws.rounding,ws.background );
	nk_widget_text (&win->buffer,warnwidpos,warning,nk_strlen (warning),&warningtext,NK_TEXT_CENTERED,ctx->style.font);
}
