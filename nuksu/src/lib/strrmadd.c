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

#include <stdlib.h>  // for size_t
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
