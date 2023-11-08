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


#ifndef WIDGETS_H
	#define WIDGETS_H


struct password
{
	char * buf;
	size_t bufsize;
	size_t length;
};
struct passwords_input_data
{

	bool active ;
	size_t cursor_pos;
	size_t shift;
	bool showpassword ;
	long lastt;
	int lockopeness;
};

struct lock_style
{
	short int lock_speed;

	struct nk_color shackle;
	struct nk_color body;
	struct nk_color key_hole;
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
struct warning_style
{
	struct nk_color background;
	float rounding;
	struct nk_color text_background;
	struct nk_color text_forground;
};
void password_input (struct nk_context * ctx,struct passwords_input_data * data,struct passwords_input_style ipstyle,struct password * password,struct XWindow xw,Cursor cu);

void warning_widget (struct nk_context*ctx, struct warning_style ws, char * warning) ;
#endif
