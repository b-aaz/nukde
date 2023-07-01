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

static char filecmp (void * sort,const void * a, const void * b)
{
	struct fileinfo * file1;
	struct fileinfo * file2;
	struct sortby sb;
	sb = * (struct sortby *) sort ;
	file1=* (struct fileinfo **) a;
	file2= * (struct fileinfo **) b;

	if (file1!=NULL && file2==NULL)
	{
		return -1;
	}

	if (file1==NULL && file2!=NULL)
	{
		return 1;
	}

	if (file1==NULL && file2==NULL)
	{
		return 0;
	}

	switch (sb.st)
	{
		case NAME:
			return sb.ac*strnucmp ( file1->name, file2->name);
			break;

		case TYPE:
			return sb.ac*strnucmp ( file1->type.humanreadable, file2->type.humanreadable);
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
void sortfiles (struct fileinfo ** files, size_t * fnum , struct sortby st) {

			qsort_r(files, fnum, sizeof(struct fileinfo *),&st,filecmp);
}


