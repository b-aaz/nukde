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

#include <arpa/inet.h>            /* for ntohl, ntohs*/
#include <stdio.h>                /* for fread, fclose, FILE, size_t*/
#include <stdlib.h>               /* for malloc*/
#include <string.h>               /* for memcmp*/

#define NUERRREDEFFUNCS
#define NUERRSTDIO 
#define NUERRSTDLIB 
#include "../../../colibs/err.h"  /* for die, fopen*/


/* The header structure for reading the farbfeld format read farbfeld(5) */
static struct farbfeld_header
{
    char ma [7];
    unsigned int w;
    unsigned int h;
};
/* Reads the farbfeld image data onto memory from a file descriptor 
 * byte by byte to a RGBA array of little endian 8-bit(a unsigned 
 * char) per channel pixels and sets the with and hight.*/
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
 * the ffread function and sets the with and hight.*/
unsigned char * ffread_open(const char * path,unsigned int * w,unsigned int * h)
{
    unsigned char * buffer;
    FILE * file;
    file=fopen(path,"rb");
    buffer=ffread(file,w,h);
    fclose(file);
    return buffer;
}


