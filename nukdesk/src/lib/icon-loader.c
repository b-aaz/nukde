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

#include <GLES3/gl3.h>                /* for glTexParameterf, GL_TEXTURE_2D*/
#include <stdio.h>                    /* for printf, puts, FILE, pclose, popen*/
#include <stdlib.h>                   /* for free, malloc, NULL*/
#include <string.h>                   /* for strcat, strlen, strncmp, strcmp*/
#include <threads.h>                  /* for thrd_create, thrd_error, thrd_s...*/

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#include "../../../colibs/err.h"      /* for die*/
#include "../../../colibs/nuklear.h"  /* for nk_image, nk_image_id*/
#include "../../../colibs/bool.h"   /* for true, false, bool*/
#include "ffload.h"                   /* for ffread, ffread_open*/
#include "fileinfo_type.h"            /* for fileinfo, thrd_icon_load_args*/
#include "stb_image_resize.h"         /* for stbir_resize_uint8*/

#define ICON_W 150
unsigned char * read_buffer_resize_open(char * path,long int image_r_w,long int image_r_h)
{
    unsigned int image_w=0, image_h =0 ;
    unsigned char * image;
    unsigned char * image_r;
    image = ffread_open(path,&image_w,&image_h);
    /*simage_r=(unsigned char *)malloc(image_w*image_h*4);*/
    /*stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);*/
    image_r=(unsigned char *)malloc(image_r_w*image_r_h*4);
    stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
    free(image);
    return image_r;
}
unsigned char * read_buffer_resize(FILE * file,long int image_r_w,long int image_r_h)
{
    unsigned int image_w=0, image_h =0 ;
    unsigned char * image;
    unsigned char * image_r;
    image = ffread(file,&image_w,&image_h);
    /*image_r=(unsigned char *)malloc(image_w*image_h*4);*/
    /*stbir_resize_uint8(image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);*/
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
/*if there is no similar icons in files it starts a thered to load the image file into ram*/
void start_thrd_for_icon(struct fileinfo ** files,int fnum,int i)
{
    printf("the iterator %d name:%s type:%s:end\n",i,files[i]->name,files[i]->icon_load_args.type);
    if(strncmp(files[i]->icon_load_args.type,"img:",5)==0)
    {
        bool iconisuniqe = true;
        /*unsigned int l;*/
        /*if (areyisnew){l=i;}else{l=fnum;}*/
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


