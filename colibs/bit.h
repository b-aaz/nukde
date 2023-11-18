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


#ifndef BIT_H
	#define BIT_H

# include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#define getbit(x,i) (((x[i/CHAR_BIT])>>(i%CHAR_BIT))&(1))
#define setbit(x,i,v) 				\
x[i/CHAR_BIT] = 				\
( 						\
	       	( 				\
			(x[i/CHAR_BIT]) 	\
			& 			\
			(~(1<<(i%CHAR_BIT))) 	\
	       	) 				\
						\
						\
	       	| (v<<(i%CHAR_BIT))		\
	       					\
)						\

static size_t countbitsset ( unsigned char * x , size_t bytes)
{
	size_t count=0;
	for (size_t byte=0; byte < bytes;byte++)
	{
		unsigned char b = x[byte];
		for (;b;count++)
		{
			b&=b-1;
		}
	}
	return count;
}

static void printbyte(unsigned char byte){
	char i=0;
	printf("0b");
	for (;i<CHAR_BIT;i++){
		printf("%d",byte&1);
		byte>>=1;
	}
	printf(" ");
}


#endif 
