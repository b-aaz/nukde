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


#include <stdio.h>                /* for NULL, size_t*/
#include <stdlib.h>               /* for free, malloc*/

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#include <string.h>               /* for strncpy, strlen, strncmp, strstr*/

#include "../../../colibs/err.h"  /* for die*/
#include "fileinfo_type.h"

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
 * For now it's used for icon configuration files to specify the type of .
 * A specific file-type's icon image .
 * "img" being the type for a static image .
 * "eie" being the type for a program which is executed with the file's path to generate an image .
 * 2-The options
 * It's a word that will be searched for in a configuration file to get its value .
 * 3-The suboptions
 * OK this is a bit complicated
 * This feature is only used in icon configuration files and it's here because of limitations of mime types .
 * The mime standard has most popular file formats covered but we need more!
 * For example the farbfeld format has no unique mime type and it's identified with the "application/octet-stream" mime type witch
 * Is the mime type for many other "obscure" file formats not identified by the standard
 * But libmagic provides a human readable identifier for files and it has a wiiiiiiiiiide range of file formats supported
 * So here's where suboptions come in handy .
 * We take a file and give it to libmagic and get its human-readable identifier and its mime id .
 * We search the configuration file for the lines having the mime type in there option find .
 * We then go in it's suboptions and for each suboption we store it and search the human readable identifier for any occurrence of it .
 * If there was a occurrence we return the suboptions value .
 * And if there was no occurrence in all suboptions the first found option(i .e the normal mime identifier  is returned .
 */

/*Gets a type in a string form and returns it as a enum .*/
/*For "enum icon_type" see fileinfo_type.h .*/
static enum icon_type parse_icon_type (char * chtype, unsigned int typel)
{
	if (strncmp (chtype,"img:",typel) ==0)
	{
		return IMG;
	}
	else if (strncmp (chtype,"eie:",typel) ==0)
	{
		return EIE;
	}
	else
	{
		die ("%s\n : ", "Unknown Icon Type", chtype);
	}
}
/*TODO:Use memmem instead of strstr*/
/*TODO:Use an enum for the types*/
static char * check_subops (char * bigpattern,char * data,enum icon_type * type)
{
	size_t pl=0, linelength=0;
	char * pattern=NULL;
	char * line=NULL;
	size_t n=0;
	unsigned int typel=4;

	while (data[n]==',')
	{
		++n;
		linelength=0;

		if (type!=NULL)
		{
			n+=typel;
		}

		while (data[n+pl]!='=')
		{
			++pl;
		}

		if (NULL!=pattern)
		{
			free (pattern);
		}

		pattern = malloc (pl+1);
		strncpy (pattern,data+n,pl);
		pattern[pl]='\0';
		n++;
		n+=pl;

		while (data[n+linelength] != '\n')
		{
			++linelength;
		}

		if (strstr (bigpattern,pattern) != NULL)
		{
			line=malloc (linelength+1);
			strncpy (line,data+n,linelength);
			line[linelength]='\0';

			if (type!=NULL)
			{
				type= parse_icon_type	(data+n-pl-typel-1,typel);
			}

			return line;
			free (pattern);
			pattern=NULL;
		}

		n+=linelength+1;
	}

	return line;
}
/*Gets an option value in a given line*/
static char * line_parse (size_t * n,char * data,size_t * linelength)
{
	char * line=NULL;

	if (data[*n]!='=')
	{
		die ("Syntax error at %lu",*n);
	}

	++*n;

	while (data[*n+*linelength] != '\n')
	{
		++*linelength;
	}

	if (0!=*linelength)
	{
		line=malloc (*linelength+1);
		strncpy (line,data+*n,*linelength);
		line[*linelength]='\0';
	}

	return line ;
}
/* Searches for and returns the value of option .
 * The type argument should be pre allocated .
 * The "type" argument can be set to "NULL" for configuration files with out types .
 * The "bigoption" argument can be set to "NULL" for configuration files with out suboptions .
 * */
char * get_config (char * data, char * option,char * bigoption,enum icon_type * type)
{
	size_t n=0,pl=0, dl=0,linelength=0;
	char * line=NULL;
	char * lineops=NULL;
	unsigned int typel=0;
	pl=strlen (option);
	dl=strlen (data);

	if (type!=NULL)
	{
		typel=4;
	}

	while (n<dl)
	{
		if (n==0||data[n-1]=='\n')
		{
			if (strncmp (data+n+typel,option,pl) ==0)
			{
				n+=typel;
				n+=pl;
				line=line_parse (&n,data,&linelength);

				if (bigoption != NULL)
				{
					if (data[n+linelength+1]==',')
					{
						lineops= check_subops (bigoption,data+n+linelength+1,type);

						if (lineops!=NULL)
						{
							return lineops;
						}
					}
				}

				if (type!=NULL)
				{
					*type=parse_icon_type (data+n-typel-pl-1,typel);
				}

				return line;
			}
		}

		n++;
	}

	return NULL;
}
