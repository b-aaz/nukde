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


#ifndef BIT_H
	#define BIT_H

# include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#define getbit(x,i) ((x[i/CHAR_BIT])>>(i%CHAR_BIT))&(1)
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
