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
#include <ctype.h>
int strnucmp(short int *ac,const char * str1,const char * str2)
{
	size_t numlength=0;
	while(1)
	{
out:
		if(*str1=='\0' && !(*str2=='\0'))
		{
			return -*ac;
		}
		if(!(*str1=='\0') && *str2=='\0')
		{
			return *ac;
		}
		if(*str1=='\0' && *str2=='\0')
		{
			return 0;
		}
		if(isdigit(*str1)&&isdigit(*str2))
		{
			while(1)
			{
				if(isdigit(*str1)&& !isdigit(*str2))
				{
					return *ac;
				}
				if(!isdigit(*str1)&&isdigit(*str2))
				{
					return -*ac;
				}
				if(!isdigit(*str1)&&!isdigit(*str2))
				{
					str1-=numlength;
					str2-=numlength;
					while(1)
					{
						if(!isdigit(*str1)&&!isdigit(*str2))
						{
							numlength=0;
							goto out;
						}
						if(*str1>*str2)
						{
							return *ac;
						}
						if(*str1<*str2)
						{
							return -*ac;
						}
						str1 ++;
						str2 ++;
					}
				}
				str1++;
				str2++;
				numlength++;
			}
		}
		else if(isdigit(*str1)&& !isdigit(*str2))
		{
			return -*ac;
		}
		else if(!isdigit(*str1)&&isdigit(*str2))
		{
			return *ac;
		}
		else
		{
			if(*str1>*str2)
			{
				return *ac;
			}
			if(*str1<*str2)
			{
				return -*ac;
			}
		}
		str1++;
		str2++;
	}
}
