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
/*
 * These functions DO NOT handle the memory reallocation themselves .
 */

/* Removes |num| chars starting at the |pos| position from the string |str|
 * with the length of |strl| .
 */
char * strrm (char * str,size_t strl,size_t pos,size_t num)
{
	for (; pos<strl ; pos++)
	{
		str[pos]=str[pos+num];
	}

	str[pos]='\0';
	return str;
}

/* Inserts the string |stradd| with the length |straddl| at the |pos| position
 * to the string |str| with the length |strl| .
 *
 * You need to allocate sufficient memory at |str| to hold |strl|+|straddl|+1
 * failure to do so will result in UB .  
 */
char * stradd (char * str,size_t strl,char * stradd,size_t straddl,size_t pos)
{
	do
	{
		str[straddl+strl]=str[strl];
	}
	while (strl>pos&&--strl);

	while (straddl-->0)
	{
		str[pos+straddl]=stradd[straddl];
	}

	return str;
}
