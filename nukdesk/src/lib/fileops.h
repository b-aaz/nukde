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
struct magicline
{
    char * humanreadable;
    char * mime;
    char * encode;
};
struct thrd_icon_load_args
{
    bool genid;
    thrd_t thrd;
    char * type;
    char * icon_path;
    long int icon_size;
    unsigned char * return_data;
} ;
struct fileinfo
{
    bool deletded;
    bool isselected;
    struct kevent f_change;
    struct kevent f_event;
    int fd;
    char * name;
    char * description;
    char * path;
    struct nk_image * return_image;
    long long unsigned int f_size;
    struct stat f_stat;
    struct thrd_icon_load_args icon_load_args;
    struct magicline magic;
};
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
bool input_mouse_has_clicked_even_times_in_rect(struct nk_input * in,enum nk_buttons id,struct nk_rect rect,bool * downup);
int pos_is_in_rect(struct nk_vec2 v,struct nk_rect r);
void lunch(char * openbuff,struct fileinfo f);
void draw_icon(struct nk_context * ctx,char * name,struct fileinfo * file,char * openbuff,struct nk_rect icrect);
int intcmp(int ac,long long int a, long long int b);
int pstrcmp(void * sort,const void * a, const void * b);
unsigned char * read_buffer_resize_open(char * path,long int image_r_w,long int image_r_h);
unsigned char * read_buffer_resize(FILE * file,long int image_r_w,long int image_r_h);
struct nk_image load_buffer_resize(unsigned char * image,unsigned int  image_w,unsigned int image_h, long int image_r_w,long int image_r_h);
struct nk_image load_image_file_resize(FILE * file,long int image_r_w,long int image_r_h);
struct nk_image load_image_open_resize(char * path,long int image_r_w,long int image_r_h);
struct nk_image load_open_icon(char * path, long int icon_size);
struct nk_image load_file_icon(FILE * file, long int icon_size);
char * fileopentobuff(const char * path);
void magic_line_split(const	char * data, char * lines[]);
unsigned long long int dirsize(char * dirpath);
int ifbuffisadhortcut(char * buff);
int thrd_icon_load(void * args);
int thrd_icon_load_from_extion(void * args);
void start_thrd_for_icon(struct fileinfo ** files,int fnum,int i);
struct fileinfo * new_file(char * d_path,char * name, char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr);
void delete_file(struct fileinfo ** files, int fnum, int i);
struct fileinfo ** updatefiles(struct dsk_dir desktop_dir,unsigned int * fnum,int kqueue,struct fileinfo ** files,char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr);
void loadicon(struct fileinfo * file);
