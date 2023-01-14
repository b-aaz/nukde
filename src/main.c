/*
Copyright 2022 B-aaz

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

#include <sys/event.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <magic.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <arpa/inet.h>
#include <sys/endian.h>
#include <dirent.h>
#include <sys/stat.h>
#include <magic.h>
#include <threads.h>
#include <X11/Xlib.h>
#define REDEFFUNCS
#include "libs/err.h"
#define FAILSAFEICON "./examples/pics/f.ff"
#define FAILSAFEICONIDX "./examples/iconidx.idx"
#define FAILSAFEOPENIDX "./examples/openidx.idx"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_XLIB_GL3_IMPLEMENTATION
#define NK_XLIB_LOAD_OPENGL_EXTENSIONS
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "libs/stb_image_resize.h"
//#include "../../nuklear.h"
#include <nuklear.h>
#include "libs/nuklear_xlib_gl3.h"
#include "libs/nusort.h"
#include <GL/glext.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024


#define ICON_W 150
#define MINLFPAD 30
#define MINRIPAD 30
#define ICONHPAD 20

typedef enum  {false, true} boolean;
boolean input_mouse_has_clicked_even_times_in_rect(struct nk_input * in,enum nk_buttons id,struct nk_rect rect,boolean * downup)
{
	if(nk_input_is_mouse_click_in_rect(in,id,rect))
	{
		*downup=!*downup;
	}
	return *downup;
}


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
static int gl_err = nk_false;
static int gl_error_handler(Display * dpy, XErrorEvent * ev)
{
	NK_UNUSED(dpy);
	NK_UNUSED(ev);
	gl_err = nk_true;
	return 0;
}

static void
die(const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputs("\n", stderr);
	exit(EXIT_FAILURE);
}

static int
has_extension(const char * string, const char * ext)
{
	const char * start, *where, *term;
	where = strchr(ext, ' ');
	if(where || *ext == '\0')
		return nk_false;
	for(start = string;;)
	{
		where = strstr((const char *)start, ext);
		if(!where) break;
		term = where + strlen(ext);
		if(where == start || *(where - 1) == ' ')
		{
			if(*term == ' ' || *term == '\0')
				return nk_true;
		}
		start = term;
	}
	return nk_false;
}




struct head
{
	char ma [7];
	unsigned int w;
	unsigned int h;
};

unsigned char * ffread(FILE * file,unsigned int * w,unsigned int * h)
{
	struct head hh= {0};
	size_t filelen;
	unsigned char * buffer;
	unsigned short  temp=0;
	size_t i=0;
	//hh=malloc(16);
	fread(&hh,16,1,file);
	if(strcmp("farbfeld",hh.ma))
	{
		puts("Invalid file format1");
		//		exit(1);
	}
	*w=ntohl(hh.w);
	*h=ntohl(hh.h);
	printf("%d %d\n",*w,*h);
	filelen=(*w)*(*h)*4;
	buffer=malloc(filelen*sizeof(char));
	for(; i<filelen; i++)
	{
		fread(&temp,2,1,file);//==1)//{
		buffer[i]=(ntohs(temp)/257);//}
	}
	return buffer;
}

unsigned char * ffread_open(const char * path,unsigned int * w,unsigned int * h)
{
	unsigned char * buffer;
	FILE * file;
	file=fopen(path,"rb");
	buffer=ffread(file,w,h);
	fclose(file);
	return buffer;
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

struct magicline
{
	char * humanreadable;
	char * mime;
	char * encode;
};

struct thrd_icon_load_args
{
	boolean genid;
	thrd_t thrd;
	char * type;
	char * icon_path;
	long int icon_size;
	unsigned char * return_data;
} ;
struct fileinfo
{
	boolean deletded;
	boolean isselected;
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

char * line_parse(size_t * n,char * data,size_t * linelength)
{
	char * line=NULL;
	if(data[*n]!='=')
	{
		printf("syntax erorr at %lu",*n);
		exit(EXIT_FAILURE);
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
			puts("error no programs found");
			exit(EXIT_FAILURE);
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


struct menupos
{
	boolean isactive;
	struct nk_vec2 pos;
};

void nicon(struct nk_context * ctx,char * name,struct fileinfo * file,char * openbuff,struct nk_rect icrect)
{
	/*





	     enum nk_widget_layout_states state;
	    state = nk_widget(&bounds, ctx);
	    if (!state) return 0;
	    in = (state == NK_WIDGET_ROM || layout->flags & NK_WINDOW_ROM) ? 0 : &ctx->input;
	*/
	//nk_layout_row_dynamic(ctx, 30, 2);
	//nk_button_label(ctx, "button");
	struct nk_window * win;
	struct nk_text text ;
	struct nk_input * in;
	in =  &ctx->input;
	text.padding =nk_vec2(0,0);
	text.background = nk_rgb(0,0,0);
	text.text = nk_rgb(250,250,250);
	win = ctx->current;
	if(in)
	{
		if(nk_input_is_mouse_hovering_rect(in,icrect))
		{
			if(in->mouse.buttons[NK_BUTTON_LEFT].down)
			{
				nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,0,42));
				if(in->mouse.buttons[NK_BUTTON_LEFT].clicked)
				{
					puts("gg");
					lunch(openbuff, *file);
				}
			}
			//the shadow arond the icon when cursor is on it
			nk_stroke_rect(&win->buffer,icrect,0,2,nk_rgba(250,250,250,32));
			nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,0,32));
		}
	}
	nk_widget_text(&win->buffer,icrect,name,nk_strlen(name),&text,NK_TEXT_ALIGN_CENTERED|NK_TEXT_ALIGN_BOTTOM,ctx->style.font);
	nk_draw_image(&win->buffer,nk_rect(icrect.x,icrect.y,icrect.w,icrect.w),file->return_image,nk_rgb(255,255,255));
}


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
	if(fp==NULL)
	{
		printf("cant open %s",path);
	}
	fseek(fp,0l,SEEK_END);
	fl=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	buff=malloc(fl+1);
	fread(buff,fl, 1,fp);
	fclose(fp);
	buff[fl]='\0';
	printf(" fileopentobuff start|%s|end\n[7~",buff);
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
	printf("the itrator %d name:%s type:%s:end\n",i,files[i]->name,files[i]->icon_load_args.type);
	if(strncmp(files[i]->icon_load_args.type,"img:",5)==0)
	{
		boolean iconisuniqe = true;
		//unsigned int l;
		//if (areyisnew){l=i;}else{l=fnum;}
		for(unsigned int i2 = 0; i2<fnum; i2++)
		{
			printf("the itrator of the internal for  %d the file itrator %u\n",i2,i);
			if(strcmp(files[i2]->icon_load_args.icon_path,files[i]->icon_load_args.icon_path)==0)
			{
				iconisuniqe=false;
				puts("got into  cpy");
				printf("coping icon from a file with itrator of %d, the name of %s and icon path of %s : %s for use at the file with the itrator of %d and name of  %s and icon path of %s : %s \n",i2,files[i2]->name,files[i2]->icon_load_args.icon_path,files[i2]->icon_load_args.type,           i,files[i]->name,files[i]->icon_load_args.icon_path,files[i]->icon_load_args.type);
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
				printf("could not make thread for %d\n",i);
				exit(0);
			}
		}
	}
	if(strncmp(files[i]->icon_load_args.type,"eie:",5)==0)
	{
		puts("got into eie");
		files[i]->return_image=malloc(sizeof(struct nk_image));
		if(thrd_create(&(files[i]->icon_load_args.thrd),(thrd_start_t) thrd_icon_load_from_extion,(files[i]))==thrd_error)
		{
			printf("could not make thread for %d\n",i);
			exit(0);
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
	printf(" 2File deleted %s\n",files[i]->name);
	free(files[i]->path);
	free(files[i]->magic.mime);
	free(files[i]->magic.encode);
	free(files[i]->name);
	free(files[i]);
	files[i]=NULL;
	puts("deled");
}

struct charnode
{
	char * name;
	struct charnode * next;
};



struct fileinfo ** updatefiles(struct dsk_dir desktop_dir,unsigned int * fnum,int kqueue,struct fileinfo ** files,char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr)
{
	//puts("upp\n");
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
	//evevent itrator
	for(int ei=0; ei<nev; ei++)
	{
		printf("ei %d\n",ei);
		//event file itrator
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
			perror("kevent");
			exit(0);
		}
		if(eventlist[ei].fflags & NOTE_EXTEND)
		{
			printf("2File extended %s\n",files[efi]->name);
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
			printf("2File closed %s\n",files[efi]->name);
		}
		if(eventlist[ei].fflags & NOTE_CLOSE_WRITE)
		{
			printf("2File closed and writed %s\n",files[efi]->name);
		}
		if(eventlist[ei].fflags & NOTE_LINK)
		{
			printf("2File links changed %s\n",files[efi]->name);
		}
		if(eventlist[ei].fflags & NOTE_OPEN)
		{
			printf("2File opened %s\n",files[efi]->name);
		}
		if(eventlist[ei].fflags & NOTE_READ)
		{
			printf("2File was reed %s\n",files[efi]->name);
		}
		if(eventlist[ei].fflags & NOTE_REVOKE)
		{
			printf("2File revoked %s\n",files[efi]->name);
		}
		if(eventlist[ei].fflags & NOTE_ATTRIB)
		{
			printf("2File attributes modified %s\n",files[efi]->name);
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
						for(int i =0; i< oldfnum; i++)
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

int main(void)
{
	unsigned int de_hight,de_width;
	struct nk_image  bgimage;
	int running = 1;
	struct XWindow win;
	GLXContext glContext;
	struct nk_context * ctx;
	struct nk_colorf bg;
	char * folder="/desktop";
	char * hpath;
	int kqid;
	struct nk_vec2 lastpos;
	struct dsk_dir desktop_dir;
	struct dirent * dir ;
	char * iconidx;
	char * openidx;
	magic_t magic_cookie_mime=0;
	magic_t magic_cookie_hr=0;
	kqid=kqueue();
	if(kqid == -1)
	{
		perror("kqueue\n");
	}
	magic_cookie_mime = magic_open(MAGIC_MIME|MAGIC_PRESERVE_ATIME|MAGIC_SYMLINK|MAGIC_MIME_TYPE);
	magic_cookie_hr = magic_open(MAGIC_NONE);
	if(magic_cookie_mime == NULL)
	{
		printf("unable to initialize magic library\n");
		exit(EXIT_FAILURE);
	}
	printf("Loading default magic database\n");
	if(magic_load(magic_cookie_mime, NULL) != 0)
	{
		printf("cannot load magic database - %s\n", magic_error(magic_cookie_mime));
		magic_close(magic_cookie_mime);
		exit(EXIT_FAILURE);
	}
	if(magic_cookie_hr == NULL)
	{
		printf("unable to initialize magic library\n");
		exit(EXIT_FAILURE);
	}
	printf("Loading default magic database\n");
	if(magic_load(magic_cookie_hr, NULL) != 0)
	{
		printf("cannot load magic database - %s\n", magic_error(magic_cookie_hr));
		magic_close(magic_cookie_hr);
		exit(EXIT_FAILURE);
	}
	memset(&win, 0, sizeof(win));
	win.dpy = XOpenDisplay(NULL);
	if(!win.dpy) die("Failed to open X display\n");
	{
		int glx_major, glx_minor;
		if(!glXQueryVersion(win.dpy, &glx_major, &glx_minor))
			die("[X11]: Error: Failed to query OpenGL version\n");
		if((glx_major == 1 && glx_minor < 3) || (glx_major < 1))
			die("[X11]: Error: Invalid GLX version!\n");
	}
	{
		int fb_count;
		static GLint attr[] =
		{
			GLX_X_RENDERABLE,   True,
			GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
			GLX_RENDER_TYPE,    GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
			GLX_RED_SIZE,       8,
			GLX_GREEN_SIZE,     8,
			GLX_BLUE_SIZE,      8,
			GLX_ALPHA_SIZE,     8,
			GLX_DEPTH_SIZE,     24,
			GLX_STENCIL_SIZE,   8,
			GLX_DOUBLEBUFFER,   True,
			None
		};
		GLXFBConfig * fbc;
		fbc = glXChooseFBConfig(win.dpy, DefaultScreen(win.dpy), attr, &fb_count);
		if(!fbc) die("[X11]: Error: failed to retrieve framebuffer configuration\n");
		{
			int i;
			int fb_best = -1, best_num_samples = -1;
			for(i = 0; i < fb_count; ++i)
			{
				XVisualInfo * vi = glXGetVisualFromFBConfig(win.dpy, fbc[i]);
				if(vi)
				{
					int sample_buffer, samples;
					glXGetFBConfigAttrib(win.dpy, fbc[i], GLX_SAMPLE_BUFFERS, &sample_buffer);
					glXGetFBConfigAttrib(win.dpy, fbc[i], GLX_SAMPLES, &samples);
					if((fb_best < 0) || (sample_buffer && samples > best_num_samples))
						fb_best = i, best_num_samples = samples;
				}
			}
			win.fbc = fbc[fb_best];
			XFree(fbc);
			win.vis = glXGetVisualFromFBConfig(win.dpy, win.fbc);
		}
	}
	{
		win.cmap = XCreateColormap(win.dpy, RootWindow(win.dpy, win.vis->screen), win.vis->visual, AllocNone);
		win.swa.colormap =  win.cmap;
		win.swa.background_pixmap = None;
		win.swa.border_pixel = 0;
		win.swa.event_mask =
		    ExposureMask | KeyPressMask | KeyReleaseMask |
		    ButtonPress | ButtonReleaseMask| ButtonMotionMask |
		    Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
		    PointerMotionMask| StructureNotifyMask;
		de_hight=DisplayHeight(win.dpy,DefaultScreen(win.dpy));
		de_width=DisplayWidth(win.dpy,DefaultScreen(win.dpy));
		win.win = XCreateWindow(win.dpy, RootWindow(win.dpy, win.vis->screen), 0, 0,
		                        de_width, de_hight, 0, win.vis->depth, InputOutput,
		                        win.vis->visual, CWBorderPixel|CWColormap|CWEventMask, &win.swa);
		if(!win.win) die("[X11]: Failed to create window\n");
		XFree(win.vis);
		XStoreName(win.dpy, win.win, "desk");
		XMapWindow(win.dpy, win.win);
		win.wm_delete_window = XInternAtom(win.dpy, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(win.dpy, win.win, &win.wm_delete_window, 1);
	}
	{
		typedef GLXContext(*glxCreateContext)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
		int(*old_handler)(Display *, XErrorEvent *) = XSetErrorHandler(gl_error_handler);
		const char * extensions_str = glXQueryExtensionsString(win.dpy, DefaultScreen(win.dpy));
		glxCreateContext create_context = (glxCreateContext)
		                                  glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
		gl_err = nk_false;
		if(!has_extension(extensions_str, "GLX_ARB_create_context") || !create_context)
		{
			fprintf(stderr, "[X11]: glXCreateContextAttribARB() not found...\n");
			fprintf(stderr, "[X11]: ... using old-style GLX context\n");
			glContext = glXCreateNewContext(win.dpy, win.fbc, GLX_RGBA_TYPE, 0, True);
		}
		else
		{
			GLint attr[] =
			{
				GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
				GLX_CONTEXT_MINOR_VERSION_ARB, 0,
				None
			};
			glContext = create_context(win.dpy, win.fbc, 0, True, attr);
			XSync(win.dpy, False);
			if(gl_err || !glContext)
			{
				attr[1] = 1;
				attr[3] = 0;
				gl_err = nk_false;
				fprintf(stdout, "[X11] Failed to create OpenGL 3.0 context\n");
				fprintf(stdout, "[X11] ... using old-style GLX context!\n");
				glContext = create_context(win.dpy, win.fbc, 0, True, attr);
			}
		}
		XSync(win.dpy, False);
		XSetErrorHandler(old_handler);
		if(gl_err || !glContext)
			die("[X11]: Failed to create an OpenGL context\n");
		glXMakeCurrent(win.dpy, win.win, glContext);
	}
	ctx = nk_x11_init(win.dpy, win.win);
	{
		struct nk_font_atlas * atlas;
		nk_x11_font_stash_begin(&atlas);
		nk_x11_font_stash_end();
	}
	iconidx=fileopentobuff(FAILSAFEICONIDX);
	openidx=fileopentobuff(FAILSAFEOPENIDX);
	hpath=getenv("HOME");
	desktop_dir.d_path =malloc(strlen(hpath)+strlen(folder)+1);
	strcpy(desktop_dir.d_path,hpath);
	strcat(desktop_dir.d_path,folder);
	desktop_dir.d_open = open(desktop_dir.d_path, O_RDONLY);
	EV_SET(&desktop_dir.d_change,desktop_dir.d_open, EVFILT_VNODE,
	       EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR,
	       NOTE_WRITE,
	       0, 0);
	unsigned int fnum=0;
	desktop_dir.d=opendir(desktop_dir.d_path);
	if(desktop_dir.d)
	{
		while((dir=readdir(desktop_dir.d))!= NULL)
		{
			if(!(!strcmp(".",dir->d_name)||!strcmp("..",dir->d_name)))
			{
				fnum++;
			}
		}
	}
	else
	{
		puts("No desktop folder");
		exit(EXIT_FAILURE);
	}
	struct fileinfo ** files;
	printf("%d\n",fnum);
	files=malloc(fnum * sizeof(struct fileinfo *));
	size_t fi=0;
	rewinddir(desktop_dir.d);
	while((dir=readdir(desktop_dir.d))!= NULL)
	{
		if(!(!strcmp(".",dir->d_name)||!strcmp("..",dir->d_name)))
		{
			files[fi]=new_file(desktop_dir.d_path,dir->d_name,iconidx,magic_cookie_mime,magic_cookie_hr);
			fi++;
		}
	}
	puts("icon thrding , io , cp and loading eie started !!!!!!!!!");
	for(unsigned int i = 0; i<fnum ; i++)
	{
		printf("fis %d\n",i) ;
		start_thrd_for_icon(files,i,i);
	}
	struct sortby st;
	st.ac=1;
	st.st=NAME;
	qsort_r(files, fnum, sizeof(struct fileinfo *),&st,pstrcmp);
	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
	bgimage=load_image_open_resize(FAILSAFEICON, de_width,de_hight);
	struct menupos menupos;
	menupos.isactive=false;
	while(running)
	{
		XEvent evt;
		nk_input_begin(ctx);
		while(XPending(win.dpy))
		{
			XNextEvent(win.dpy, &evt);
			if(evt.type == ClientMessage) goto cleanup;
			if(XFilterEvent(&evt, win.win)) continue;
			nk_x11_handle_event(&evt);
		}
		nk_input_end(ctx);
		ctx->style.window.padding = nk_vec2(0,0);
		ctx->style.window.spacing = nk_vec2(0,0);
		ctx->style.window.scrollbar_size = nk_vec2(0,0);
		if(nk_begin(ctx, "desk", nk_rect(0, 0, de_width, de_hight),0))
		{
			files= updatefiles(desktop_dir,&fnum,kqid,files,iconidx,magic_cookie_mime,magic_cookie_hr);
			struct nk_window * win;
			win = ctx->current;
			nk_draw_image(&win->buffer,nk_rect(0,0,de_width,de_hight),&bgimage,nk_rgb(255,255,255));
			int row=0;
			unsigned int col=0;
			unsigned int maximum_cols=(de_width-(MINLFPAD+MINRIPAD))/(ICON_W+ICONHPAD);
			float calpad = (float)((de_width-(MINLFPAD+MINRIPAD)) %(ICON_W+ICONHPAD)) /2;
			//printf ("%f\n", calpad) ;
			nk_layout_space_begin(ctx, NK_STATIC, 70,maximum_cols);
			while(row<(fnum/maximum_cols)+1)
			{
				col=0;
				while((col+(row*maximum_cols))<fnum&&col<maximum_cols)
				{
					int iconnum=col+(row*maximum_cols);
					//				printf("%d %d\n",col+(i*maximum_cols),fnum);
					struct nk_rect icrect = nk_rect((col*(ICONHPAD+ICON_W)+MINLFPAD+calpad),(row* (ICON_W+20+50))+10, ICON_W, ICON_W+20);
					if(files[iconnum]->icon_load_args.genid)
					{
						loadicon(files[iconnum]);
					};
					nicon(ctx,files[iconnum]->name,(files[iconnum]),openidx,icrect);
					if(nk_input_is_mouse_click_in_rect(&ctx->input,NK_BUTTON_RIGHT,icrect))
					{
						//int sif=0;
						if(nk_input_is_key_down((&ctx->input),NK_KEY_SHIFT))
						{
							files[iconnum]->isselected=!files[iconnum]->isselected;
						}
						else
						{
							menupos.isactive=true;
							menupos.pos.x=*(&ctx->input.mouse.pos.x);
							menupos.pos.y=*(&ctx->input.mouse.pos.y);
						}
					}
					if(nk_input_is_mouse_click_in_rect(&ctx->input,NK_BUTTON_MIDDLE,icrect))
					{
						files[iconnum]->isselected=!files[iconnum]->isselected;
					}
					if(files[iconnum]->isselected)
					{
						printf("sel%d\n",col+(row*maximum_cols));
						nk_stroke_rect(&win->buffer,icrect,0,2,nk_rgba(250,250,250,32));
						nk_fill_rect(&win->buffer,icrect,0,nk_rgba(0,0,255,32));
					}
					col++;
				}
				row++;
			}
			if(nk_input_has_mouse_click(&ctx->input,0))
			{
				lastpos.x=*(&ctx->input.mouse.pos.x);
				lastpos.y=*(&ctx->input.mouse.pos.y);
			}
		}
		nk_end(ctx);
		if(nk_input_is_mouse_hovering_rect(&ctx->input,nk_rect(menupos.pos.x,menupos.pos.y, 100, 180)))
		{
			if(menupos.isactive)
			{
				//puts("heeeeheheheh");
				if(nk_begin(ctx, "menu", nk_rect(menupos.pos.x,menupos.pos.y, 100, 180),0))
				{
					nk_layout_row_dynamic(ctx,30,1);
					if(nk_button_label(ctx,"delete"))
					{
						menupos.isactive=false;
						int sif=0;
						while(sif<fnum)
						{
							if(files[sif]->isselected==true)
							{
								remove(files[sif]->path);
							}
							files[sif]->isselected=false;
							sif++;
						}
					}
					nk_layout_row_dynamic(ctx,30,1);
					nk_button_label(ctx,"rename");
					nk_layout_row_dynamic(ctx,30,1);
					nk_button_label(ctx,"move");
					nk_layout_row_dynamic(ctx,30,1);
					nk_button_label(ctx,"lunch");
					nk_layout_row_dynamic(ctx,30,1);
					nk_button_label(ctx,"open with");
					nk_layout_row_dynamic(ctx,30,1);
					nk_button_label(ctx,"propertys");
					nk_end(ctx);
				}
			}
		}
		else
		{
			menupos.isactive=false;
		}
		XGetWindowAttributes(win.dpy, win.win, &win.attr);
		glViewport(0, 0, win.width, win.height);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(bg.r, bg.g, bg.b, bg.a);
		nk_x11_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		glXSwapBuffers(win.dpy, win.win);
	}
cleanup:
	nk_x11_shutdown();
	glXMakeCurrent(win.dpy, 0, 0);
	glXDestroyContext(win.dpy, glContext);
	XUnmapWindow(win.dpy, win.win);
	XFreeColormap(win.dpy, win.cmap);
	XDestroyWindow(win.dpy, win.win);
	XCloseDisplay(win.dpy);
	return 0;
}
