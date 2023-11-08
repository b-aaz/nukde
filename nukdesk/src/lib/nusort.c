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

