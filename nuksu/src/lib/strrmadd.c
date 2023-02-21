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
char *strrm(char * str,size_t strl,size_t idx,int num)
{
    for(; idx<strl ;idx++)
    {
        str[idx]=str[idx+num];
    }
    str[idx]='\0';
    return str;

}
char *stradd(char * str,int strl,char * stradd,int straddl,int idx)
{
    int j = 0;
    for(; strl >= idx; strl--)
    {
        str[straddl+strl]=str[strl];
    }
    for(; j<straddl; j++)
    {
        str[idx+j]=stradd[j];
    }
    return str;
}
