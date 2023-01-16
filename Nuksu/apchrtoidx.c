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

#include <stdio.h>
#include <stdlib.h>
#include "apchrtoidx.h"
char * apchrtoidx(char * str,int strl,char * ch,int chl,int idx)
{
	int i= strl;
	int j = 0;
	for(; i >= idx; i--)
	{
		str[i+chl]=str[i];
	}
	for(; j<chl; j++)
	{
		str[idx+j]=ch[j];
	}
	return str;
}
