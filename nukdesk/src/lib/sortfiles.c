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
#include<stdio.h>
#include<stdlib.h>
#include "fileinfo_type.h"        /* for fileinfo, thrd_icon_load_args, magi...*/

#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#include "../../../colibs/err.h"  /* for die, fopen*/
#include "nusort.h"               /* for strnucmp*/
#include "sortfiles.h"               /* for strnucmp*/
static char intcmp (int ac,long long int a, long long int b)
{
	if (a>b)
	{
		return ac;
	}
	else if (a<b)
	{
		return -ac;
	}
	else
	{
		return 0;
	}
}

static char filecmp (const void * a, const void * b)
{
	struct fileinfo * file1;
	struct fileinfo * file2;
	extern struct sortby sb;
	file1=* (struct fileinfo **) a;
	file2= * (struct fileinfo **) b;

	switch (sb.st)
	{
		case NAME:
			return sb.ac*strnucmp (file1->name, file2->name);
			break;

		case TYPE:
			return sb.ac*strnucmp (file1->type.humanreadable, file2->type.humanreadable);
			break;

		case SIZE:
			return intcmp (sb.ac, file1->f_size, file2->f_size);
			break;

		case MTIME:
			return intcmp (sb.ac, file1->f_stat.st_mtime, file2->f_stat.st_mtime);
			break;

		case CTIME:
			return intcmp (sb.ac, file1->f_stat.st_ctime, file2->f_stat.st_ctime);
			break;

		case ATIME:
			return intcmp (sb.ac, file1->f_stat.st_atime, file2->f_stat.st_atime);
			break;

		default:
			break;
	}
}
void sortfiles (struct fileinfo ** files, size_t  fnum, struct sortby st)
{
mergesort (files,fnum, sizeof (struct fileinfo *),filecmp);
}


