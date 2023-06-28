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
/* This function compares two strings in a natural user friendly
 * fashion that is nicer for sorting files names in a file manager,
 * desktop, sorting items for a menu and etc.

 * Instead of just comparing each chars ASCII value from
 * the two strings (like what strcmp() does) , this functions takes
 * numbers that are in the strings into account too.

 * For example consider the following strings:
 * 12.png
 * 1.png
 * 100.png
 * 2023.png
 * 5.png

 * If we sorted this strings with a sorting algorithm
 * and provided strcmp as its comparison function the
 * results would be similar to the flowing:
 * 5.png
 * 2023.png
 * 12.png
 * 100.png
 * 1.png

 * And that's counter intuitive.
 * But instead if you provided strnucmp 
 * 1.png
 * 5.png
 * 12.png
 * 100.png
 * 2023.png

 * And That's a natural NUmerical comparison.
 */
#define TT  3
#define TF  2
#define FT  1
#define FF  0
int strnucmp (const char * str1,const char * str2)
{
	register unsigned char cm; /* This char stores the result of two CoMparisons in its first two bits*/
	size_t numlength=0; /* Stores the number of digits of a NUMber in both the strings */

	while (1)
	{
out: /* Label to get OUT of the inner loop */


		cm = 0;
		cm = *str1=='\0';
		cm <<= 1;
		cm |= *str2=='\0';

		switch (cm)
		{
			case TF :
				return -1;

			case FT :
				return 1;

			case TT :
				return 0;
		}

		cm = 0;
		cm =isdigit (*str1);
		cm <<= 1;
		cm |=isdigit (*str2);

		switch (cm)
		{
			case FF :
				break;

			case TF :
				return -1;

			case FT :
				return 1;

			case TT:
				while (1)
				{
					str1++;
					str2++;
					numlength++;

					cm = 0;
					cm =isdigit (*str1);
					cm <<= 1;
					cm |=isdigit (*str2);

					switch (cm)
					{
						case TT :
							break;

						case FF :
							str1-=numlength;
							str2-=numlength;

							while (numlength-->0)
							{
								if (*str1>*str2)
								{
									return 1;
								}

								if (*str1<*str2)
								{
									return -1;
								}

								str1 ++;
								str2 ++;
							}

							goto out;

						case TF :
							return 1;

						case FT :
							return -1;
					}
				}
		}

		if (*str1>*str2)
		{
			return 1;
		}

		if (*str1<*str2)
		{
			return -1;
		}

		str1++;
		str2++;
	}
}

