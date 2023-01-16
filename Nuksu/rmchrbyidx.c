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

#include <stdlib.h>
#include <string.h>
char * rmchrbyidx(char * c,int index,int numtoremove)
{
	size_t i = index;
	char * ctemp = (char *) malloc(strlen(c)+1);
	strcpy(ctemp,c);
	for(; i<strlen(c); i++)
	{
		c[i]=c[i+numtoremove];
	}
	c[i]='\0';
	return c;
}
