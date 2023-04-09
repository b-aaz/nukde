#include <stdio.h>                // for NULL, size_t
#include <stdlib.h>               // for free, malloc

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#define NUERRCOLOR ""
#define NUERRCOLORRE ""
#include <string.h>               // for strncpy, strlen, strncmp, strstr

#include "../../../colibs/err.h"  // for die

/*
 *The configuration lines of the "config" files have a syntax link this:
 *
 * type:option=something
 * ,suboption=something
 * ,suboption=something
 * ,suboption=something
 */
/*TODO:use memmem instead of strstr*/
static char * check_subops(char * bigpattern,char * data,char * type)
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
    return line;
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

char * get_config(char * data, char * pattern,char * bigpattern,char * type)
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
                        lineops= check_subops(bigpattern,data+n+linelength+1,type);
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
