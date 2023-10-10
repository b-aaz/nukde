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

#ifndef ICON_LOADER_H
	#define ICON_LOADER_H


unsigned char * read_buffer_resize_open(char * path,long int image_r_w,long int image_r_h);
unsigned char * read_buffer_resize(FILE * file,long int image_r_w,long int image_r_h);
struct nk_image load_buffer_resize(unsigned char * image,unsigned int  image_w,unsigned int image_h, long int image_r_w,long int image_r_h);
struct nk_image load_image_file_resize(FILE * file,long int image_r_w,long int image_r_h);
struct nk_image load_image_open_resize(char * path,long int image_r_w,long int image_r_h);
struct nk_image load_open_icon(char * path, long int icon_size);
struct nk_image load_file_icon(FILE * file, long int icon_size);
int thrd_icon_load(void * args);
int thrd_icon_load_from_extion(void * args);
void start_thrd_for_icon(struct fileinfo ** files,int fnum,int i);
void loadicon(struct fileinfo * file);

#endif 
