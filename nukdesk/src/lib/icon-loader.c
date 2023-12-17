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

#include <GLES3/gl3.h>                /* for glTexParameterf, GL_TEXTURE_2D*/
#ifdef DEBUG
	#include <stdio.h>                    /* for printf, puts, FILE, pclose, popen*/
#endif
#include <stdlib.h>                   /* for free, malloc, NULL*/
#include <string.h>                   /* for strcat, strlen, strcmp, strcmp*/
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
unsigned char * read_buffer_resize_open (char * path,long int image_r_w,long int image_r_h)
{
	unsigned int image_w=0, image_h =0 ;
	unsigned char * image;
	unsigned char * image_r;
	image = ffread_open (path,&image_w,&image_h);
	image_r= (unsigned char *) malloc (image_r_w*image_r_h*4);
	stbir_resize_uint8 (image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
	free (image);
	return image_r;
}
unsigned char * read_buffer_resize (FILE * file,long int image_r_w,long int image_r_h)
{
	unsigned int image_w=0, image_h =0 ;
	unsigned char * image;
	unsigned char * image_r;
	image = ffread (file,&image_w,&image_h);
	image_r= (unsigned char *) malloc (image_r_w*image_r_h*4);
	stbir_resize_uint8 (image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
	free (image);
	return image_r;
}



struct nk_image load_buffer_resize (unsigned char * image,unsigned int  image_w,unsigned int image_h, long int image_r_w,long int image_r_h)
{
	unsigned char * image_r;
	GLuint tex;
	image_r= (unsigned char *) malloc (image_r_w*image_r_h*4);
	stbir_resize_uint8 (image,image_w,image_h,0,image_r,image_r_w,image_r_h,0,4);
	glGenTextures (1, &tex);
	glBindTexture (GL_TEXTURE_2D, tex);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, image_r_w, image_r_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_r);
	glGenerateMipmap (GL_TEXTURE_2D);
	free (image);
	free (image_r);
	return nk_image_id ( (int) tex);
}

struct nk_image load_image_file_resize (FILE * file,long int image_r_w,long int image_r_h)
{
	unsigned int w=0,h=0;
	unsigned char * image;
	image=ffread (file,&w,&h);
	return load_buffer_resize (image,w,h,image_r_w,image_r_h);
}

struct nk_image load_image_open_resize (char * path,long int image_r_w,long int image_r_h)
{
	unsigned int w=0,h=0;
	unsigned char * image;
	image=ffread_open (path,&w,&h);
	return load_buffer_resize (image,w,h,image_r_w,image_r_h);
}



struct nk_image load_open_icon (char * path, long int icon_size)
{
	return load_image_open_resize (path,icon_size,icon_size);
}
struct nk_image load_file_icon (FILE * file, long int icon_size)
{
	return load_image_file_resize (file,icon_size,icon_size);
}

int thrd_icon_load (void * args)
{
	struct thrd_icon_load_args * sargs =args;
#ifdef DEBUG
	printf ("Thread started for %s\n",sargs->icon_path);
#endif
	sargs->return_data= read_buffer_resize_open (sargs->icon_path,sargs->icon_size,sargs->icon_size);
#ifdef DEBUG
	printf ("Thread ended for %s\n",sargs->icon_path);
#endif
	sargs->generateid=true;
	return 0;
}

int thrd_icon_load_from_extion (void * args)
{
	struct fileinfo * file =args;
	char * ic_gen_with_args=malloc (strlen (file->icon_load_args.icon_path)+1+strlen (file->path)+1);
#ifdef DEBUG
	puts ("EIE started\n");
#endif
	strcpy (ic_gen_with_args,file->icon_load_args.icon_path);
	strcat (ic_gen_with_args," ");
	strcat (ic_gen_with_args,file->path);
#ifdef DEBUG
	printf ("ic_gen_with_args %s\n",ic_gen_with_args);
#endif
	FILE * fileimage;
	fileimage=popen (ic_gen_with_args,"r");

	if (fileimage==NULL)
	{
		die ("%s\n","Can't open the process");
	}

	file->icon_load_args.return_data =read_buffer_resize (fileimage,file->icon_load_args.icon_size,file->icon_load_args.icon_size);
	pclose (fileimage);
	free (ic_gen_with_args);
#ifdef DEBUG
	puts ("EIE done\n");
#endif
	file->icon_load_args.generateid=true;
	return 0;
}
/*if there is no similar icons in files it starts a thread to load the image file into ram*/
void start_thrd_for_icon (struct fileinfo ** files,int fnum,int i)
{
#ifdef DEBUG
	printf ("the iterator %d name:%s type:%d:end\n",i,files[i]->name,files[i]->icon_load_args.icon_type);
#endif

	if (files[i]->icon_load_args.icon_type==IMG)
	{
		bool iconisuniqe = true;

		for (unsigned int i2 = 0; i2<fnum; i2++)
		{
			if (strcmp (files[i2]->icon_load_args.icon_path,files[i]->icon_load_args.icon_path) ==0)
			{
				iconisuniqe=false;
#ifdef DEBUG
				puts ("COPY");
				printf ("Coping icon from a file with iterator of %d, the name of %s and icon path of %s : %s for use at the file with the iterator of %d and name of  %s and icon path of %s : %d \n",i2,files[i2]->name,files[i2]->icon_load_args.icon_path,files[i2]->icon_load_args.icon_type,           i,files[i]->name,files[i]->icon_load_args.icon_path,files[i]->icon_load_args.icon_type);
#endif
				++*files[i2]->ic_copy_count;
				(files[i]->return_image) = (files[i2]->return_image);
				(files[i]->ic_copy_count) = (files[i2]->ic_copy_count);
				break;
			}
		}

		if (iconisuniqe)
		{
#ifdef DEBUG
			puts ("IMG thread :");
			printf ("%s\n",files[i]->name);
#endif
			files[i]->return_image= calloc (1,sizeof (struct nk_image));
			files[i]->ic_copy_count= calloc (1,sizeof (unsigned int));

			if (thrd_create (& (files[i]->icon_load_args.thrd), (thrd_start_t) thrd_icon_load,& (files[i]->icon_load_args)) ==thrd_error)
			{
				die ("Could not make a thread for %d\n",i);
			}
		}
	}

	if (files[i]->icon_load_args.icon_type==EIE)
	{
#ifdef DEBUG
		puts ("EIE:");
#endif
		files[i]->return_image= calloc (1,sizeof (struct nk_image));
		files[i]->ic_copy_count= NULL ;

		if (thrd_create (& (files[i]->icon_load_args.thrd), (thrd_start_t) thrd_icon_load_from_extion, (files[i])) ==thrd_error)
		{
			die ("Could not make a thread for %d\n",i);
		}
	}
}



void generateid (struct fileinfo * file)
{
#ifdef DEBUG
	printf ("Starting to generate image for : %s\n",file->name);
#endif
	GLuint tex;
	glGenTextures (1, &tex);
	glBindTexture (GL_TEXTURE_2D, tex);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, ICON_W,ICON_W, 0, GL_RGBA, GL_UNSIGNED_BYTE, file->icon_load_args.return_data);
	glGenerateMipmap (GL_TEXTURE_2D);
	free (file->icon_load_args.return_data);
	* (file->return_image) =nk_image_id ( (int) tex);
	file->icon_load_args.generateid=false;
#ifdef DEBUG
	printf ("Generating image for : %s is done\n",file->name);
#endif
}


