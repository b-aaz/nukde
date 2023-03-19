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

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <sys/event.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <magic.h>
#include <threads.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <string.h>
#include <GL/glxext.h>
#include <GLES3/gl3.h>

#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xlocale.h>

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#define NUERRCOLOR ""
#define NUERRCOLORRE ""
#include "../../../colibs/err.h"
#define FAILSAFEICON "../examples/pics/err.ff"
#define BGIMAGE "../examples/pics/bg.ff"
#define FAILSAFEICONIDX "../examples/icon.nucfg"
#define FAILSAFEOPENIDX "../examples/open.nucfg"
#include "stb_image_resize.h"
#define NK_PRIVET
//  implementation 
#include "../../../colibs/nuklear.h"
#include "nuklear_xlib_gl3.h"
#include "nusort.h"
#include "ffload.h"

#define ICON_W 150

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#include "fileops.h"
boolean input_mouse_has_clicked_even_times_in_rect(struct nk_input * in,enum nk_buttons id,struct nk_rect rect,boolean * downup)
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
char * line_parse(size_t * n,char * data,size_t * linelength)
{
    char * line=NULL;
    if(data[*n]!='=')
    {
        die("Syntax error at %lu",*n);
    }
    ++*n;
    while(data[*n+*linelength] != '\n')
    {
        ++*linelength;
    }
    if(0!=*linelength)
    {
        line=malloc(*linelength+1);
        strncpy(line,data+*n,*linelength);
        line[*linelength]='\0';
    }
    return line ;
}

char * check_ops(char * bigpattern,char * data,char * type)
{
    size_t pl=0, linelength=0;
    char * pattern=NULL;
    char * line=NULL;
    size_t n=0;
    int typel=4;
    while(data[n]==',')
    {
        ++n;
        linelength=0;
        if(type!=NULL)
        {
            n+=typel;
        }
        while(data[n+pl]!='=')
        {
            ++pl;
        }
        if(NULL!=pattern)
        {
            free(pattern);
        }
        pattern = malloc(pl+1);
        strncpy(pattern,data+n,pl);
        pattern[pl]='\0';
        n++;
        n+=pl;
        while(data[n+linelength] != '\n')
        {
            ++linelength;
        }
        if(strstr(bigpattern,pattern) != NULL)
        {
            line=malloc(linelength+1);
            strncpy(line,data+n,linelength);
            line[linelength]='\0';
            if(type!=NULL)
            {
                strncpy(type,data+n-pl-typel-1,4);
                type[4]='\0';
            }
            return line;
            free(pattern);
            pattern=NULL;
        }
        n+=linelength+1;
    }
    //	free(pattern);
    return line;
}


char * line_wswpt(char * data, char * pattern,char * bigpattern,char * type)
{
    size_t n=0,pl=0, dl=0,linelength=0;
    char * line=NULL;
    char * lineops=NULL;
    short unsigned int typel=0;
    pl=strlen(pattern);
    dl=strlen(data);
    if(type!=NULL)
    {
        typel=4;
    }
    while(n<dl)
    {
        if(n==0||data[n-1]=='\n')
        {
            if(strncmp(data+n+typel,pattern,pl)==0)
            {
                n+=typel;
                n+=pl;
                line=line_parse(&n,data,&linelength);
                if(bigpattern != NULL)
                {
                    //				n+=linelength+1;
                    if(data[n+linelength+1]==',')
                    {
                        lineops= check_ops(bigpattern,data+n+linelength+1,type);
                        if(lineops!=NULL)
                        {
                            return lineops;
                        }
                    }
                }
                if(type!=NULL)
                {
                    strncpy(type,data+n-typel-pl-1,4);
                    type[4]='\0';
                }
                return line;
            }
        }
        n++;
    }
    return NULL;
}

void lunch(char * openbuff,struct fileinfo f)
{
    char * programname;
    char * lunchpath;
    printf("mime%s:end\n",f.magic.mime);
    programname = line_wswpt(openbuff,f.magic.mime,f.magic.humanreadable,NULL);
    printf("name%s\n",programname);
    if(programname==NULL)
    {
        programname = line_wswpt(openbuff,"default",NULL,NULL);
        if(programname==NULL)
        {
            die("%s\n","No programs found");
        }
    }
    lunchpath = malloc(strlen(programname)+1+strlen(f.path)+2);
    strcpy(lunchpath,programname);
    strcat(lunchpath," ");
    strcat(lunchpath,f.path);
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
        //the shadow arond the icon when cursor is on it
        nk_stroke_rect(&win->buffer,icrect,0,2,nk_rgba(250,250,250,32));
        nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,0,32));
    }
    nk_widget_text(&win->buffer,icrect,name,nk_strlen(name),&text,NK_TEXT_ALIGN_CENTERED|NK_TEXT_ALIGN_BOTTOM,ctx->style.font);
    nk_draw_image(&win->buffer,nk_rect(icrect.x,icrect.y,icrect.w,icrect.w),file->return_image,nk_rgb(255,255,255));
}

int intcmp(int ac,long long int a, long long int b)
{
    if(a>b)
    {
        return ac;
    }
    else if(a<b)
    {
        return -ac;
    }
    else
    {
        return 0;
    }
}

int pstrcmp(void * sort,const void * a, const void * b)
{
    struct fileinfo * file1;
    struct fileinfo * file2;
    struct sortby sb;
    sb = *(struct sortby *)sort ;
    file1=* (struct fileinfo **)a;
    file2= *(struct fileinfo **)b;
    if(file1!=NULL && file2==NULL)
    {
        return -1;
    }
    if(file1==NULL && file2!=NULL)
    {
        return 1;
    }
    if(file1==NULL && file2==NULL)
    {
        return 0;
    }
    switch(sb.st)
    {
    case NAME:
        return strnucmp(&sb.ac, file1->name, file2->name);
        break;
    case TYPE:
        return strnucmp(&sb.ac, file1->magic.humanreadable, file2->magic.humanreadable);
        break;
    case SIZE:
        return intcmp(sb.ac, file1->f_size, file2->f_size);
        break;
    case MTIME:
        return intcmp(sb.ac, file1->f_stat.st_mtime, file2->f_stat.st_mtime);
        break;
    case CTIME:
        return intcmp(sb.ac, file1->f_stat.st_ctime, file2->f_stat.st_ctime);
        break;
    case ATIME:
        return intcmp(sb.ac, file1->f_stat.st_atime, file2->f_stat.st_atime);
        break;
    default:
        break;
    }
}




unsigned char * read_buffer_resize_open(char * path,long int image_r_w,long int image_r_h)
{
    unsigned int image_w=0, image_h =0 ;
    unsigned char * image;
    unsigned char * image_r;
    image = ffread_open(path,&image_w,&image_h);
    //simage_r=(unsigned char *)malloc(image_w*image_h*4);
    //stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
    image_r=(unsigned char *)malloc(image_r_w*image_r_h*4);
    stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
    free(image);
    return image_r;
}
/*unsigned char * buffer_resize(unsigned char * ffbuf,long int image_r_w,long int image_r_h)
{
	unsigned int image_w=0, image_h =0 ;
	unsigned char * image;
	unsigned char * image_r;
	image = ffread_buf(ffbuf,&image_w,&image_h);
	//image_r=(unsigned char *)malloc(image_w*image_h*4);
	//stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
	image_r=(unsigned char *)malloc(image_r_w*image_r_h*4);
	stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
	free(image);
	return image_r;
}*/
unsigned char * read_buffer_resize(FILE * file,long int image_r_w,long int image_r_h)
{
    unsigned int image_w=0, image_h =0 ;
    unsigned char * image;
    unsigned char * image_r;
    image = ffread(file,&image_w,&image_h);
    //image_r=(unsigned char *)malloc(image_w*image_h*4);
    //stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
    image_r=(unsigned char *)malloc(image_r_w*image_r_h*4);
    stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
    free(image);
    return image_r;
}



struct nk_image load_buffer_resize(unsigned char * image,unsigned int  image_w,unsigned int image_h, long int image_r_w,long int image_r_h)
{
    unsigned char * image_r;
    GLuint tex;
    image_r=(unsigned char *)malloc(image_r_w*image_r_h*4);
    stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image_r_w, image_r_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_r);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(image);
    free(image_r);
    return nk_image_id((int)tex);
}

struct nk_image load_image_file_resize(FILE * file,long int image_r_w,long int image_r_h)
{
    unsigned int w=0,h=0;
    unsigned char * image;
    image=ffread(file,&w,&h);
    return load_buffer_resize(image,w,h,image_r_w,image_r_h);
}

struct nk_image load_image_open_resize(char * path,long int image_r_w,long int image_r_h)
{
    unsigned int w=0,h=0;
    unsigned char * image;
    image=ffread_open(path,&w,&h);
    return load_buffer_resize(image,w,h,image_r_w,image_r_h);
}



struct nk_image load_open_icon(char * path, long int icon_size)
{
    return load_image_open_resize(path,icon_size,icon_size);
}
struct nk_image load_file_icon(FILE * file, long int icon_size)
{
    return load_image_file_resize(file,icon_size,icon_size);
}


char * fileopentobuff(const char * path)
{
    FILE * fp;
    size_t fl;
    char * buff;
    fp=fopen(path,"r");
    fseek(fp,0l,SEEK_END);
    fl=ftell(fp);
    fseek(fp,0L,SEEK_SET);
    buff=malloc(fl+1);
    fread(buff,fl, 1,fp);
    fclose(fp);
    buff[fl]='\0';
    return buff;
}







void magic_line_split(const	char * data, char * lines[])
{
    size_t n=0;
    size_t linelength=0;
    while(data[linelength] != ';')
    {
        if(data[linelength]=='\0')
        {
            puts("lolo");
        }
        linelength++;
    }
    lines[0]=malloc(linelength+1);
    memcpy(lines[0],data,linelength);
    lines[0][linelength]='\0';
    n+=linelength;
    linelength=0;
    n+=9;
    if(data[n]!='=')
    {
        puts("lolo1");
    }
    n++;
    while(data[n+linelength] != '\0')
    {
        linelength++;
    }
    lines[1]=malloc(linelength+1);
    memcpy(lines[1],data+n,linelength);
    lines[1][linelength]='\0';
}

//gets the size of a directory with stat and recurion
unsigned long long int dirsize(char * dirpath)
{
    unsigned long long int size=0;
    DIR * dir;
    struct dirent * f;
    char * fpath;
    struct stat fstat;
    dir=opendir(dirpath);
    while((f=readdir(dir))!=NULL)
    {
        if(!(!strcmp(".",f->d_name)||!strcmp("..",f->d_name)))
        {
            fpath=malloc(strlen(dirpath)+strlen(f->d_name)+2);
            strcpy(fpath,dirpath);
            strcat(fpath,"/");
            strcat(fpath,f->d_name);
            puts(fpath);
            stat(fpath,&fstat);
            if(f->d_type==4)
            {
                size+=dirsize(fpath);
            }
            if(f->d_type==8)
            {
                size+=fstat.st_size;
            }
            free(fpath);
        }
    }
    closedir(dir);
    return size;
}

int ifbuffisadhortcut(char * buff)
{
    int bn=0;
    int bnlinelength=0;
    char * sline;
    int rturn;
    while(buff[bn]!='\n')
    {
        bn++;
    }
    bn++;
    while(buff[bn+bnlinelength]!='\n')
    {
        bnlinelength++;
    }
    sline = malloc(bnlinelength+1);
    strncpy(sline,buff+bn,bnlinelength);
    sline[bnlinelength]='\0';
    rturn=	memcmp(sline,"#shortcut",bnlinelength);
    free(sline);
    return rturn;
}

int thrd_icon_load(void * args)
{
    struct thrd_icon_load_args * sargs =args;
    printf("thread started for %s\n",sargs->icon_path);
    sargs->return_data= read_buffer_resize_open(sargs->icon_path,sargs->icon_size,sargs->icon_size);
    printf("thread ended for %s\n",sargs->icon_path);
    sargs->genid=true;
    return 0;
}

int thrd_icon_load_from_extion(void * args)
{
    struct fileinfo * file =args;
    char * ic_gen_with_args=malloc(strlen(file->icon_load_args.icon_path)+1+strlen(file->path)+1);
    puts("eie started\n");
    strcpy(ic_gen_with_args,file->icon_load_args.icon_path);
    strcat(ic_gen_with_args," ");
    strcat(ic_gen_with_args,file->path);
    printf("ic_gen_with_args %s\n",ic_gen_with_args);
    FILE * fileimage;
    fileimage=popen(ic_gen_with_args,"r");
    if(fileimage==NULL)
    {
        puts("cant open the proses");
    }
    file->icon_load_args.return_data =read_buffer_resize(fileimage,file->icon_load_args.icon_size,file->icon_load_args.icon_size);
    pclose(fileimage);
    free(ic_gen_with_args);
    puts("eie done\n");
    file->icon_load_args.genid=true;
    return 0;
}
//if there is no smilar icons in files it starts a thered to load the image file into ram
void start_thrd_for_icon(struct fileinfo ** files,int fnum,int i)
{
    printf("the iterator %d name:%s type:%s:end\n",i,files[i]->name,files[i]->icon_load_args.type);
    if(strncmp(files[i]->icon_load_args.type,"img:",5)==0)
    {
        boolean iconisuniqe = true;
        //unsigned int l;
        //if (areyisnew){l=i;}else{l=fnum;}
        for(unsigned int i2 = 0; i2<fnum; i2++)
        {
            printf("the iterator of the internal for  %d the file iterator %u\n",i2,i);
            if(strcmp(files[i2]->icon_load_args.icon_path,files[i]->icon_load_args.icon_path)==0)
            {
                iconisuniqe=false;
                puts("got into  cpy");
                printf("coping icon from a file with iterator of %d, the name of %s and icon path of %s : %s for use at the file with the iterator of %d and name of  %s and icon path of %s : %s \n",i2,files[i2]->name,files[i2]->icon_load_args.icon_path,files[i2]->icon_load_args.type,           i,files[i]->name,files[i]->icon_load_args.icon_path,files[i]->icon_load_args.type);
                (files[i]->return_image)=(files[i2]->return_image);
                break;
            }
        }
        if(iconisuniqe)
        {
            puts("got into  thrd");
            printf("thrd loading for %s\n",files[i]->name);
            files[i]->return_image= malloc(sizeof(struct nk_image));
            if(thrd_create(&(files[i]->icon_load_args.thrd),(thrd_start_t) thrd_icon_load,&(files[i]->icon_load_args))==thrd_error)
            {
                die("Could not make a thread for %d\n",i);
            }
        }
    }
    if(strncmp(files[i]->icon_load_args.type,"eie:",5)==0)
    {
        puts("got into eie");
        files[i]->return_image=malloc(sizeof(struct nk_image));
        if(thrd_create(&(files[i]->icon_load_args.thrd),(thrd_start_t) thrd_icon_load_from_extion,(files[i]))==thrd_error)
        {
            die("Could not make a thread for %d\n",i);
        }
    }
}

struct fileinfo * new_file(char * d_path,char * name, char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr)
{
    struct fileinfo * file ;
    char * shortcutfilebuffer;
    char * lines[2];
    file= malloc(sizeof(struct fileinfo));
    file->icon_load_args.type=malloc(4+1);
    file->icon_load_args.icon_size=ICON_W;
    file->icon_load_args.genid=false;
    file-> isselected=false;
    puts("||||||newfile down||||||||");
    puts(name);
    file->name= malloc(strlen(name)+10);
    strcpy(file->name,name);
    file->name[strlen(name)]='\0';
    printf("name %s\n",file->name);
    file->path=malloc(strlen(d_path)+strlen(name)+2);
    strcpy(file->path,d_path);
    strcat(file->path,"/");
    strcat(file->path,name);
    file->fd=open(file->path,O_RDONLY);
    EV_SET(&file->f_change, file->fd, EVFILT_VNODE,
           EV_ADD | EV_ENABLE | EV_ONESHOT|EV_CLEAR,
           NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE | NOTE_ATTRIB | NOTE_CLOSE | NOTE_CLOSE_WRITE|NOTE_LINK|NOTE_OPEN|NOTE_READ|NOTE_RENAME,
           0, 0);
    stat(file->path,&file->f_stat);
    printf("name: %s\n",file->name);
    if(S_ISDIR(file->f_stat.st_mode))
    {
        file->f_size=dirsize(file->path);
    }
    else
    {
        file->f_size=file->f_stat.st_size;
    }
    file->magic.humanreadable=magic_file(magic_cookie_hr,file->path);
    magic_line_split(magic_file(magic_cookie_mime,file->path),lines);
    file->magic.mime=lines[0];
    file->magic.encode=lines[1];
    file->icon_load_args.icon_path=NULL;
    if(strcmp(file->magic.mime,"text/x-shellscript")==0)
    {
        shortcutfilebuffer=fileopentobuff(file->path);
        if(ifbuffisadhortcut(shortcutfilebuffer)==0)
        {
            strcpy(file->icon_load_args.type,"img:");
            file->icon_load_args.type[4]='\0';
            file->icon_load_args.icon_path=line_wswpt(shortcutfilebuffer,"#icon",NULL,NULL);
            file->description=line_wswpt(shortcutfilebuffer,"#description",NULL,NULL);
        }
    }
    if(file->icon_load_args.icon_path == NULL)
    {
        file->icon_load_args.icon_path=line_wswpt(iconidx,file->magic.mime,file->magic.humanreadable,file->icon_load_args.type);
        if(file->icon_load_args.icon_path==NULL)
        {
            puts("failsafeL0");
            file->icon_load_args.icon_path=line_wswpt(iconidx,file->magic.encode,NULL,file->icon_load_args.type);
            if(file->icon_load_args.icon_path==NULL)
            {
                puts("failsafeL1");
                file->icon_load_args.icon_path=FAILSAFEICON;
                strcpy(file->icon_load_args.type,"img:");
                file->icon_load_args.type[4]='\0';
            }
        }
    }
    printf("icpath: %s , type: %s\n",file->icon_load_args.icon_path,file->icon_load_args.type);
    return file;
}


void delete_file(struct fileinfo ** files, int fnum, int i)
{
    if(strcmp(files[i]->icon_load_args.type,"eie:")==0)
    {
        thrd_join(files[i]->icon_load_args.thrd,NULL);
        free(files[i]->icon_load_args.icon_path);
        free(files[i]->icon_load_args.type);
        if(files[i]->icon_load_args.genid)
        {
            free(files[i]->icon_load_args.return_data);
        }
        else
        {
            glDeleteTextures(1, &files[i]->return_image->handle.id);
        }
    }
    if(strcmp(files[i]->icon_load_args.type,"img:")==0)
    {
        boolean iconisuniqe = true;
        for(unsigned int i2 = 0; i2<fnum; i2++)
        {
            printf("%ud\n",i2);
            if(strcmp(files[i2]->icon_load_args.icon_path,files[i]->icon_load_args.icon_path)==0&& i != i2)
            {
                iconisuniqe=false;
                break;
            }
        }
        if(iconisuniqe)
        {
            thrd_join(files[i]->icon_load_args.thrd,NULL);
            free(files[i]->icon_load_args.icon_path);
            free(files[i]->icon_load_args.type);
            if(files[i]->icon_load_args.genid)
            {
            }
            else
            {
                glDeleteTextures(1,&files[i]->return_image->handle.id);
            }
        }
    }
    printf(" File deleted %s\n",files[i]->name);
    free(files[i]->path);
    free(files[i]->magic.mime);
    free(files[i]->magic.encode);
    free(files[i]->name);
    free(files[i]);
    files[i]=NULL;
    puts("deled");
}


struct fileinfo ** updatefiles(struct dsk_dir desktop_dir,unsigned int * fnum,int kqueue,struct fileinfo ** files,char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr)
{
    struct timespec t;
    t.tv_nsec = 0;
    t.tv_sec =0;
    int deledfiles=0;
    boolean updir=false;
    struct kevent eventlist[1]= {0};
    /*struct kevent eventlist[*fnum+1];
    struct kevent changelist[*fnum+1];
    for(size_t i=0; i< (*fnum); i++)
    {
    	changelist[i]=files[i]->f_change;
    }
    changelist[*fnum]=desktop_dir.d_change;*/
    int nev;
    //struct kevent eventd;
    nev = kevent(kqueue, &desktop_dir.d_change, 1,eventlist, 1, &t);
    //evevent iterator
    for(int ei=0; ei<nev; ei++)
    {
        printf("ei %d\n",ei);
        //event file iterator
        int efi=0;
        for(; efi< (*fnum); efi++)
        {
            if(files[efi]->fd==eventlist[ei].ident)
            {
                break;
            }
        }
        if(nev == -1)
        {
            die("%s","Kevent");
        }
        if(eventlist[ei].fflags & NOTE_EXTEND)
        {
            printf("File extended %s\n",files[efi]->name);
        }
        if(eventlist[ei].fflags & NOTE_WRITE)
        {
            puts("heay");
            if(eventlist[ei].ident==desktop_dir.d_open)
            {
                updir=true;
            }
        }
        if(eventlist[ei].fflags& NOTE_CLOSE)
        {
            printf("File closed %s\n",files[efi]->name);
        }
        if(eventlist[ei].fflags & NOTE_CLOSE_WRITE)
        {
            printf("File closed and writed %s\n",files[efi]->name);
        }
        if(eventlist[ei].fflags & NOTE_LINK)
        {
            printf("File links changed %s\n",files[efi]->name);
        }
        if(eventlist[ei].fflags & NOTE_OPEN)
        {
            printf("File opened %s\n",files[efi]->name);
        }
        if(eventlist[ei].fflags & NOTE_READ)
        {
            printf("File was reed %s\n",files[efi]->name);
        }
        if(eventlist[ei].fflags & NOTE_REVOKE)
        {
            printf("File revoked %s\n",files[efi]->name);
        }
        if(eventlist[ei].fflags & NOTE_ATTRIB)
        {
            printf("File attributes modified %s\n",files[efi]->name);
        }
        struct sortby st;
        st.ac=1;
        st.st=NAME;
        //	qsort_r(files, *fnum, sizeof(struct fileinfo *),&st,pstrcmp);
        deledfiles=0;
        struct dirent * dir;
        //int newfilenum=0;
        if(updir)
        {
            //	sleep(1);
            int oldfnum= *fnum;
            *fnum = 0;
            puts("dir writento\n");
            rewinddir(desktop_dir.d);
            if(desktop_dir.d)
            {
                for(int i =0; i<oldfnum; i++)
                {
                    files[i]->deletded=true;
                }
                printf("old : %d\n",oldfnum);
                while((dir=readdir(desktop_dir.d))!= NULL)
                {
                    if(!(!strcmp(".",dir->d_name)||!strcmp("..",dir->d_name)))
                    {
                        (*fnum)++;
                        printf("fnumi: %d\n",*fnum);
                    }
                }
                printf("fnum: %d\n",*fnum);
                rewinddir(desktop_dir.d);
                struct charnode * new_files_names=NULL;
                while((dir=readdir(desktop_dir.d))!= NULL)
                {
                    if(!(!strcmp(".",dir->d_name)||!strcmp("..",dir->d_name)))
                    {
                        boolean isnew=true;

                        for(int i =0; i< (oldfnum-deledfiles); i++)
                        {
                            printf("i: %d dn: %s fn: %s\n",i,dir->d_name,files[i]->name);
                            if(strcmp(dir->d_name,files[i]->name)==0)
                            {
                                files[i]->deletded=false;
                                puts("file is not new");
                                isnew=false;
                                break;
                            }
                        }
                        if(isnew==true)
                        {
                            puts("file is new adding to list");
                            struct charnode * current;
                            if(new_files_names==NULL)
                            {
                                puts("empty lsit");
                                new_files_names=malloc(sizeof(struct charnode));
                                current=new_files_names;
                                current->next=NULL;
                                current->name=malloc(strlen(dir->d_name)+1);
                                strcpy(current->name,dir->d_name);
                                current->name[strlen(dir->d_name)]='\0';
                            }
                            else
                            {
                                current=new_files_names;
                                while(current->next!=NULL)
                                {
                                    current=current->next;
                                }
                                current->next=malloc(sizeof(struct charnode));
                                current->next->next=NULL;
                                current->next->name=malloc(strlen(dir->d_name)+1);
                                strcpy(current->next->name,dir->d_name);
                                current->next->name[strlen(dir->d_name)]='\0';
                            }
                            //					newfilenum++;
                        }
                    }
                }
                for(int i =0; i<oldfnum; i++)
                {
                    if(files[i]!=NULL)
                    {
                        if(files[i]->deletded==true)
                        {
                            delete_file(files,oldfnum,i);
                            for(int mm=oldfnum-1 ; mm>0; mm--)
                            {
                                if(files[mm]!=NULL)
                                {
                                    files[i]=files[mm];
                                    files[mm]=NULL;
                                    break;
                                }
                            }
                            deledfiles++;
                        }
                    }
                }
                //qsort_r(files, *fnum, sizeof(struct fileinfo *),&st,pstrcmp);
                if(*fnum!=oldfnum)
                {
                    printf("realloced from %d to %d\n", oldfnum,*fnum);
                    files=realloc(files,(*fnum) * sizeof(struct  fileinfo *));
                }
                if(new_files_names!=NULL)
                {
                    for(long int i=oldfnum-deledfiles; i<*fnum ; i++)
                    {
                        struct charnode * current;
                        printf("adding newfile to %d fnum : %d\n", i,*fnum);
                        current = new_files_names;
                        files[i]=new_file(desktop_dir.d_path,current->name,iconidx,magic_cookie_mime,magic_cookie_hr);
                        start_thrd_for_icon(files,i,i);
                        new_files_names=new_files_names->next;
                        free(current->name);
                        free(current);
                    }
                }
            }
            else
            {
                puts("no desktop folder");
            }
            printf("fnum: %d\n",*fnum);
            rewinddir(desktop_dir.d);
        }
        //qsort_r(files, *fnum, sizeof(struct fileinfo *),&st,pstrcmp);
    }
    return files;
}

void loadicon(struct fileinfo * file)
{
    printf("gen image for name : %s\n",file->name);
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ICON_W,ICON_W, 0, GL_RGBA, GL_UNSIGNED_BYTE, file->icon_load_args.return_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(file->icon_load_args.return_data);
    *(file->return_image)=nk_image_id((int)tex);
    file->icon_load_args.genid=false;
    printf("done gen image for name : %s\n",file->name);
}


