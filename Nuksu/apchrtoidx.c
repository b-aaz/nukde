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
