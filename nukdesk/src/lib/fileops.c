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
#include <dirent.h>               // for readdir, rewinddir, closedir, opendir
#include <fcntl.h>                // for open, O_RDONLY
#include <magic.h>                // for magic_file, magic_set, magic_t
#include <stdio.h>                // for printf, puts, NULL, fseek, fclose
#include <stdlib.h>               // for free, malloc, realloc
#include <string.h>               // for strcmp, strlen, strcpy, strcat, memcpy
#include <sys/dirent.h>           // for dirent
#include <sys/event.h>            // for kevent, NOTE_ATTRIB, NOTE_CLOSE
#include <sys/stat.h>             // for stat, timespec, st_atime, st_ctime
#include <threads.h>              // for thrd_join

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#define NUERRCOLOR ""
#define NUERRCOLORRE ""
#include "../../../colibs/err.h"  // for die, fopen
#include "GL/gl.h"                // for glDeleteTextures
#include "fileinfo_type.h"        // for fileinfo, thrd_icon_load_args, magi...

#define FAILSAFEICON "../examples/pics/err.ff"
#define BGIMAGE "../examples/pics/bg.ff"
#define FAILSAFEICONIDX "../examples/icon.nucfg"
#define FAILSAFEOPENIDX "../examples/open.nucfg"
#include "stb_image_resize.h"

#define NK_PRIVET
#include "config-parser.h"        // for get_config
#include "nusort.h"               // for strnucmp

#define ICON_W 150

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#include "fileops.h"




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
            file->icon_load_args.icon_path=get_config(shortcutfilebuffer,"#icon",NULL,NULL);
            file->description=get_config(shortcutfilebuffer,"#description",NULL,NULL);
        }
    }
    if(file->icon_load_args.icon_path == NULL)
    {
        file->icon_load_args.icon_path=get_config(iconidx,file->magic.mime,file->magic.humanreadable,file->icon_load_args.type);
        if(file->icon_load_args.icon_path==NULL)
        {
            puts("failsafeL0");
            file->icon_load_args.icon_path=get_config(iconidx,file->magic.encode,NULL,file->icon_load_args.type);
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
        bool iconisuniqe = true;
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
                        bool isnew=true;

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

