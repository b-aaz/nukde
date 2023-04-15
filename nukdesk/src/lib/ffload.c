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

#include <arpa/inet.h>            /* for ntohl, ntohs*/
#include <stdio.h>                /* for fread, fclose, FILE, size_t*/
#include <stdlib.h>               /* for malloc*/
#include <string.h>               /* for memcmp*/

#define NUERRREDEFFUNCS
#define NUERRSTDIO 
#define NUERRSTDLIB 
#include "../../../colibs/err.h"  /* for die, fopen*/


/*the header structure for reading the farbfeld format read farbfeld(5) */
static struct farbfeld_header
{
    char ma [7];
    unsigned int w;
    unsigned int h;
};
/* Reads the farbfeld image data onto memory from a file descriptor 
 * byte by byte to a RGBA array of little endian 8-bit(a unsigned 
 * char) per channel pixels*/
unsigned char * ffread(FILE * file,unsigned int * w,unsigned int * h)
{
    struct farbfeld_header hh= {0};
    size_t filelen;
    unsigned char * buffer;
    unsigned short  temp=0;
    size_t i=0;
    fread(&hh,16,1,file);
    if(memcmp("farbfeld",hh.ma,8)!=0)
    {
        die("%s,%s\n" , "Invalid file format",hh.ma);
    }
    *w=ntohl(hh.w);
    *h=ntohl(hh.h);
    filelen=(*w)*(*h)*4;
    buffer=malloc(filelen*sizeof(char));
    for(; i<filelen; i++)
    {
        fread(&temp,2,1,file);
        buffer[i]=(ntohs(temp)/257);
    }
    return buffer;
}
/* Opens the file in the path argument and reads the image data with the help of
 * the ffread function.*/
unsigned char * ffread_open(const char * path,unsigned int * w,unsigned int * h)
{
    unsigned char * buffer;
    FILE * file;
    file=fopen(path,"rb");
    buffer=ffread(file,w,h);
    fclose(file);
    return buffer;
}


