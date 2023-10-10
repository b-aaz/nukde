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

#ifndef FILEOPS_H
	#define FILEOPS_H

#include <dirent.h>                     /* for DIR*/
#include <magic.h>                      /* for magic_t*/
#include <sys/event.h>                  /* for kevent*/

#include "../../../colibs/bool.h"     /* for bool*/
#include "../../../colibs/nuklear.h"  /* for nk_vec2*/
#include "GL/glx.h"                     /* for GLXFBConfig, __GLXFBConfigRec*/
#include "X11/X.h"                      /* for Atom, Colormap, Window*/
#include "X11/Xlib.h"                   /* for Display, XSetWindowAttributes*/
#include "X11/Xutil.h"                  /* for XVisualInfo*/
struct dsk_dir
{
    DIR * d;
    int d_open;
    char * d_path;
    struct kevent d_change;
};
char * fileopentobuff (const char * path);
struct fileinfo * new_file(char * d_path,char * name, char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr);
void delete_file(struct fileinfo ** files, int fnum, int i);
struct fileinfo ** updatefiles(struct dsk_dir desktop_dir,unsigned int * fnum,int kqueue,struct fileinfo ** files,char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr);

#endif 
