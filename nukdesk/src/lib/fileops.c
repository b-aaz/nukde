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
/*#define NK_IMPLEMENTATION*/

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <dirent.h>               /* for readdir, rewinddir, closedir, opendir*/
#include <fcntl.h>                /* for open, O_RDONLY*/
#include <magic.h>                /* for magic_file, magic_set, magic_t*/
#include <stdio.h>                /* for printf, puts, NULL, fseek, fclose*/
#include <stdlib.h>               /* for free, malloc, realloc*/
#include <string.h>               /* for strcmp, strlen, strcpy, strcat, memcpy*/
#include <sys/dirent.h>           /* for dirent*/
#include <sys/event.h>            /* for kevent, NOTE_ATTRIB, NOTE_CLOSE*/
#include <sys/stat.h>             /* for stat, timespec, st_atime, st_ctime*/
#include <threads.h>              /* for thrd_join*/

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#include "../../../colibs/err.h"  /* for die, fopen*/
#include "GL/gl.h"                /* for glDeleteTextures*/
#include "fileinfo_type.h"        /* for fileinfo, thrd_icon_load_args, magi...*/

#define FAILSAFEICON "../examples/icons/err.ff"
#define FAILSAFEICONIDX "../examples/icon.nucfg"
#define FAILSAFEOPENIDX "../examples/open.nucfg"
#include "stb_image_resize.h"

#define NK_PRIVET
#include "config-parser.h"        /* for get_config*/
#include "icon-loader.h"        /* for start_thrd_for_icon*/

#define ICON_W 150

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#include "fileops.h"
#define DEBUG
static struct strnode
{
	char * name;
	struct strnode * next;
};
/*Reads path file into a char array */
char * fileopentobuff (const char * path)
{
	FILE * fp;
	size_t fl;
	char * buff;
	fp=fopen (path,"r");
	fseek (fp,0l,SEEK_END);
	fl=ftell (fp);
	fseek (fp,0L,SEEK_SET);
	buff=malloc (fl+1);
	fread (buff,fl, 1,fp);
	fclose (fp);
	buff[fl]='\0';
	return buff;
}
/*Splits a mime identifier returned from libmagic into a encoding and mime strings*/
static void magic_line_split (const	char * data, char * lines[])
{
	size_t n=0;
	size_t linelength=0;

	while (data[linelength] != ';')
	{
		if (data[linelength]=='\0')
		{
			die ("%s\n","Error splitting the magic line finding ';'");
		}

		linelength++;
	}

	lines[0]=malloc (linelength+1);
	memcpy (lines[0],data,linelength);
	lines[0][linelength]='\0';
	n+=linelength;
	linelength=0;
	n+=9;

	if (data[n]!='=')
	{
		die ("%s\n","Error splitting the magic line finding '='");
	}

	n++;

	while (data[n+linelength] != '\0')
	{
		linelength++;
	}

	lines[1]=malloc (linelength+1);
	memcpy (lines[1],data+n,linelength);
	lines[1][linelength]='\0';
}

/*Gets the size of a directory with stat(2) and recursion*/
static unsigned long long int dirsize (char * dirpath)
{
	unsigned long long int size=0;
	DIR * dir;
	struct dirent * f;
	char * fpath;
	struct stat fstat;
	dir=opendir (dirpath);

	while ( (f=readdir (dir)) !=NULL)
	{
		if (! (!strcmp (".",f->d_name) ||!strcmp ("..",f->d_name)))
		{
			fpath=malloc (strlen (dirpath)+strlen (f->d_name)+2);
			strcpy (fpath,dirpath);
			strcat (fpath,"/");
			strcat (fpath,f->d_name);
			puts (fpath);
			stat (fpath,&fstat);

			if (f->d_type==4)
			{
				size+=dirsize (fpath);
			}

			if (f->d_type==8)
			{
				size+=fstat.st_size;
			}

			free (fpath);
		}
	}

	closedir (dir);
	return size;
}

/*Skips a line in a file pointer*/
static int lineadvance (FILE * fp)
{
	char chr;

	while (fread (&chr,1,1,fp) ==1&&chr!='\n')
	{
	}

	return chr=='\n';
}
/*Checks if a file is a desktop shortcut.*/
static int fileisashortcut (char * path)
{
	char secondline[10];
	FILE * filepointer = fopen (path,"r");

	/*Skips the shebang line*/
	if (lineadvance (filepointer) !=0)
	{
		/*Read's the next 10 characters*/
		if (fread (secondline,1,10,filepointer) ==10)
		{
			/*Compares the 10 characters with the shortcut identifier*/
			if (memcmp (secondline,"#shortcut\n",10) ==0)
			{
				fclose (filepointer);
				return 1;
			}
		}
	}

	fclose (filepointer);
	return 0;
}
/*Adds a new file to a fileinfo * array*/
struct fileinfo * new_file (char * d_path,char * name, char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr)
{
	struct fileinfo * file ;
	char * shortcutfilebuffer;
	char * lines[2];
	file= malloc (sizeof (struct fileinfo));
	file->icon_load_args.icon_size=ICON_W;
	file->icon_load_args.generateid=false;
	file-> isselected=false;
#ifdef DEBUG
	puts ("\\/<---New file--->\\/");
	puts (name);
#endif
	file->name= malloc (strlen (name)+10);
	strcpy (file->name,name);
	file->name[strlen (name)]='\0';
#ifdef DEBUG
	printf ("Name %s\n",file->name);
#endif
	file->path=malloc (strlen (d_path)+strlen (name)+2);
	strcpy (file->path,d_path);
	strcat (file->path,"/");
	strcat (file->path,name);
	file->fd=open (file->path,O_RDONLY);
	EV_SET (&file->f_change, file->fd, EVFILT_VNODE,
			EV_ADD | EV_ENABLE | EV_ONESHOT|EV_CLEAR,
			NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE | NOTE_ATTRIB | NOTE_CLOSE | NOTE_CLOSE_WRITE|NOTE_LINK|NOTE_OPEN|NOTE_READ|NOTE_RENAME,
			0, 0);
	stat (file->path,&file->f_stat);

	if (S_ISDIR (file->f_stat.st_mode))
	{
		file->f_size=dirsize (file->path);
	}
	else
	{
		file->f_size=file->f_stat.st_size;
	}

	file->type.humanreadable=magic_file (magic_cookie_hr,file->path);
	magic_line_split (magic_file (magic_cookie_mime,file->path),lines);
	file->type.mime=lines[0];
	file->type.encode=lines[1];
	file->icon_load_args.icon_path=NULL;

	if (strcmp (file->type.mime,"text/x-shellscript") ==0)
	{
		if (fileisashortcut (file->path))
		{
			shortcutfilebuffer=fileopentobuff (file->path);
			file->icon_load_args.icon_type=IMG;
			file->icon_load_args.icon_path=get_config (shortcutfilebuffer,"#icon",NULL,NULL);
			file->description=get_config (shortcutfilebuffer,"#description",NULL,NULL);
		}
	}

	if (file->icon_load_args.icon_path == NULL)
	{
		file->icon_load_args.icon_path=get_config (iconidx,file->type.mime,file->type.humanreadable,&file->icon_load_args.icon_type);

		if (file->icon_load_args.icon_path==NULL)
		{
			file->icon_load_args.icon_path=get_config (iconidx,file->type.encode,NULL,&file->icon_load_args.icon_type);

			if (file->icon_load_args.icon_path==NULL)
			{
				file->icon_load_args.icon_path=FAILSAFEICON;
				file->icon_load_args.icon_type=IMG;
			}
		}
	}

	return file;
}
/*Deletes a file from a fileinfo * array*/
void delete_file (struct fileinfo ** files, int fnum, int i)
{
	if (files[i]->icon_load_args.icon_type==EIE)
	{
		thrd_join (files[i]->icon_load_args.thrd,NULL);
		free (files[i]->icon_load_args.icon_path);

		if (files[i]->icon_load_args.generateid)
		{
			free (files[i]->icon_load_args.return_data);
		}
		else
		{
			glDeleteTextures (1, &files[i]->return_image->handle.id);
		}
	}

	if (files[i]->icon_load_args.icon_type==IMG)
	{
		if (files[i]->ic_copy_count)
		{
			--*files[i]->ic_copy_count;
		}
		else
		{
			thrd_join (files[i]->icon_load_args.thrd,NULL);
			free (files[i]->icon_load_args.icon_path);

			if (!files[i]->icon_load_args.generateid)
			{
				glDeleteTextures (1,&files[i]->return_image->handle.id);
			}
		}
	}

#ifdef DEBUG
	printf (" File deleted %s\n",files[i]->name);
#endif
	free (files[i]->path);
	free (files[i]->type.mime);
	free (files[i]->type.encode);
	free (files[i]->name);
	free (files[i]);
	files[i]=NULL;
}
/*Updates a fileinfo * array
 * TODO:Clean up the update mechanism.
 *This function is getting really big....  */
struct fileinfo ** updatefiles (struct dsk_dir desktop_dir,unsigned int * fnum,int kqueue,struct fileinfo ** files,char * iconidx,magic_t magic_cookie_mime,magic_t magic_cookie_hr)
{
	struct timespec t;
	t.tv_nsec = 0;
	t.tv_sec =0;
	int deledfiles=0;
	bool updir=false;
	struct kevent eventlist[1]= {0};
	/*struct kevent eventlist[*fnum+1];
	struct kevent changelist[*fnum+1];
	for(size_t i=0; i< (*fnum); i++)
	{
		changelist[i]=files[i]->f_change;
	}
	changelist[*fnum]=desktop_dir.d_change;*/
	int nev;
	nev = kevent (kqueue, &desktop_dir.d_change, 1,eventlist, 1, &t);

	/*evevent iterator*/
	for (int ei=0; ei<nev; ei++)
	{
		/*event file iterator*/
		int efi=0;

		for (; efi< (*fnum); efi++)
		{
			if (files[efi]->fd==eventlist[ei].ident)
			{
				break;
			}
		}

		if (nev == -1)
		{
			die ("%s","Kevent");
		}

		if (eventlist[ei].fflags & NOTE_EXTEND)
		{
			printf ("File extended %s\n",files[efi]->name);
		}

		if (eventlist[ei].fflags & NOTE_WRITE)
		{
			/*printf ("File written to %s\n",files[efi]->name);*/
			if (eventlist[ei].ident==desktop_dir.d_open)
			{
				updir=true;
			}
		}

		if (eventlist[ei].fflags& NOTE_CLOSE)
		{
			printf ("File closed %s\n",files[efi]->name);
		}

		if (eventlist[ei].fflags & NOTE_CLOSE_WRITE)
		{
			printf ("File closed and written to %s\n",files[efi]->name);
		}

		if (eventlist[ei].fflags & NOTE_LINK)
		{
			printf ("File links changed %s\n",files[efi]->name);
		}

		if (eventlist[ei].fflags & NOTE_OPEN)
		{
			printf ("File opened %s\n",files[efi]->name);
		}

		if (eventlist[ei].fflags & NOTE_READ)
		{
			printf ("File was reed %s\n",files[efi]->name);
		}

		if (eventlist[ei].fflags & NOTE_REVOKE)
		{
			printf ("File revoked %s\n",files[efi]->name);
		}

		if (eventlist[ei].fflags & NOTE_ATTRIB)
		{
			printf ("File attributes modified %s\n",files[efi]->name);
		}

		deledfiles=0;
		struct dirent * dir;

		if (updir)
		{
			int oldfnum= *fnum;
			*fnum = 0;
			rewinddir (desktop_dir.d);

			if (desktop_dir.d)
			{
				for (int i =0; i<oldfnum; i++)
				{
					files[i]->deletded=true;
				}

				while ( (dir=readdir (desktop_dir.d)) != NULL)
				{
					if (! (!strcmp (".",dir->d_name) ||!strcmp ("..",dir->d_name)))
					{
						(*fnum)++;
					}
				}

				rewinddir (desktop_dir.d);
				struct strnode * new_files_names=NULL;
				struct strnode * tail=NULL;

				while ( (dir=readdir (desktop_dir.d)) != NULL)
				{
					if (! (!strcmp (".",dir->d_name) ||!strcmp ("..",dir->d_name)))
					{
						bool isnew=true;

						for (int i =0; i< (oldfnum-deledfiles); i++)
						{
							if (strcmp (dir->d_name,files[i]->name) ==0)
							{
								files[i]->deletded=false;
								isnew=false;
								break;
							}
						}

						if (isnew==true)
						{
							struct strnode * current;
							current=malloc (sizeof (struct strnode));
							current->next=NULL;
							current->name=malloc (strlen (dir->d_name)+1);
							strcpy (current->name,dir->d_name);
							current->name[strlen (dir->d_name)]='\0';

							if (new_files_names)
							{
								tail->next = current ;
								tail=current ;
							}
							else
							{
								tail = current ;
								new_files_names = current ;
							}
						}
					}
				}

				for (int i =0; i<oldfnum; i++)
				{
					if (files[i]!=NULL)
					{
						if (files[i]->deletded==true)
						{
							delete_file (files,oldfnum,i);

							for (int mm=oldfnum-1 ; mm>0; mm--)
							{
								if (files[mm]!=NULL)
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

				if (*fnum!=oldfnum)
				{
					files=realloc (files, (*fnum) * sizeof (struct  fileinfo *));
				}

				if (new_files_names!=NULL)
				{
					for (long int i=oldfnum-deledfiles; i<*fnum ; i++)
					{
						struct strnode * current;
						current = new_files_names;
						files[i]=new_file (desktop_dir.d_path,current->name,iconidx,magic_cookie_mime,magic_cookie_hr);
						start_thrd_for_icon (files,i,i);
						new_files_names=new_files_names->next;
						free (current->name);
						free (current);
					}
				}
			}
			else
			{
				die ("%s\n","No desktop folder");
			}

			rewinddir (desktop_dir.d);
		}
	}

	return files;
}
