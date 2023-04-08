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

     #include <sys/stat.h>
     #include <threads.h>
#include "fileinfo_type.h"
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
;
struct dsk_dir
{
    DIR * d;
    int d_open;
    char * d_path;
    struct kevent d_change;
};
struct menupos
{
    bool isactive;
    struct nk_vec2 pos;
};
enum acendeing
{
    AC=1,
    DC=-1
};
enum sorttype
{
    NAME=1,
    SIZE=2,
    TYPE=3,
    MTIME=4,
    CTIME=5,
    ATIME=6
};
struct sortby
{
    short int ac;
    enum sorttype st;
};
struct charnode
{
    char * name;
    struct charnode * next;
};
int intcmp(int ac,long long int a, long long int b);
int pstrcmp(void * sort,const void * a, const void * b);
char * fileopentobuff(const char * path);
void magic_line_split(const	char * data, char * lines[]);
unsigned long long int dirsize(char * dirpath);
int ifbuffisadhortcut(char * buff);
struct fileinfo * new_file(char * d_path,char * name, char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr);
void delete_file(struct fileinfo ** files, int fnum, int i);
struct fileinfo ** updatefiles(struct dsk_dir desktop_dir,unsigned int * fnum,int kqueue,struct fileinfo ** files,char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr);
