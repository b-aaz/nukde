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


#ifndef WIDGETS
	#define WIDGETS

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
