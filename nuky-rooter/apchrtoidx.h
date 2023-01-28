#include <stdio.h>
#include <stdlib.h>
char* apchrtoidx(char* str,int strl,char* ch,int chl,int idx) {
/*	puts("hi"); */
/*	char* str = malloc(100);;
        strcpy(str,"abcdefghijklmnop");
        int strl = 16;
        char* ch = "##";
        int chl = 2;
        int idx = 2;*/
/*	int i= strl; */
	int i= strl;
	int j = 0;
	for (; i >= idx; i--) {
/*	printf("x%d",i); */
		str[i+chl]=str[i];
	}
	for (; j<chl; j++) {
		str[idx+j]=ch[j];
	}
/*	str[idx+j+1]='\0'; */
/*		str[5]=str[5]; */

/*	printf(str); */

	return str;
}
