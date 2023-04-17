#include <stdio.h>                /* for NULL, size_t*/
#include <stdlib.h>               /* for free, malloc*/

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#include <string.h>               /* for strncpy, strlen, strncmp, strstr*/

#include "../../../colibs/err.h"  /* for die*/

/*
 *The configuration lines of the "config" files have a syntax link this:
 *
 * type:option=something
 * ,type:suboption=something
 * ,type:suboption=something
 * ,type:suboption=something
 */
/* 1-The type
 * It's a 3 letter "word" separated by a colon ':' specifying a type for its "option" .
 * For now it's used for icon configuration files to specify the type of.
 * A specific file-type's icon image .
 * "img" being the type for a static image.
 * "eie" being the type for a program which is executed with the file's path to generate an image.
 * 2-The options 
 * It's a word that will be searched for in a configuration file to get its value.
 * 3-The suboptions
 * OK this is a bit complicated 
 * This feature is only used in icon configuration files and it's here because of limitations of mime types.
 * The mime standard has most popular file formats covered but we need more!
 * For example the farbfeld format has no unique mime type and it's identified with the "application/octet-stream" mime type witch
 * Is the mime type for many other "obscure" file formats not identified by the standard 
 * But libmagic provides a human readable identifier for files and it has a wiiiiiiiiiide range of file formats supported
 * So here's where suboptions come in handy.
 * We take a file and give it to libmagic and get its human-readable identifier and its mime id.
 * We search the configuration file for the lines having the mime type in there option find.
 * We then go in it's suboptions and for each suboption we store it and search the human readable identifier for any occurrence of it.
 * If there was a occurrence we return the suboptions value .
 * And if there was no occurrence in all suboptions the first found option(i.e the normal mime identifier  is returned.
/*TODO:Use memmem instead of strstr*/
/*TODO:Use an enum for the types*/
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
/*Gets an option value in a given line*/
static char * line_parse(size_t * n,char * data,size_t * linelength)
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
/* Searches for and returns the value of option.
 * The type argument should be pre allocated.
 * The "type" argument can be set to "NULL" for configuration files with out types.
 * The "bigoption" argument can be set to "NULL" for configuration files with out suboptions.
 * */
char * get_config(char * data, char * option,char * bigoption,char * type)
{
    size_t n=0,pl=0, dl=0,linelength=0;
    char * line=NULL;
    char * lineops=NULL;
    short unsigned int typel=0;
    pl=strlen(option);
    dl=strlen(data);
    if(type!=NULL)
    {
        typel=4;
    }
    while(n<dl)
    {
        if(n==0||data[n-1]=='\n')
        {
            if(strncmp(data+n+typel,option,pl)==0)
            {
                n+=typel;
                n+=pl;
                line=line_parse(&n,data,&linelength);
                if(bigoption != NULL)
                {
                    if(data[n+linelength+1]==',')
                    {
                        lineops= check_subops(bigoption,data+n+linelength+1,type);
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
