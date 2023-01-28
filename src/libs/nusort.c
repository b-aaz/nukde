#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./natsort/strnatcmp.h"
int main(void)
{
	char * str1 = "str";
	char * str2 = "astr";
	//while (1){
	printf("%d",strnatcmp(str1,str2));
	/*if (*str1>*str2){
	puts("hi");
	exit(0);
	}
	else
	{
	puts("-1");
	exit(0);
	}
	str1++; str2++;
	}*/
}
