/*include<stdio.h>*/
#include <stdlib.h>
#include <string.h>
/*#include <unistd.h> */
char* rmchrbyidx(char* c,int index,int numtoremove) {
/*printf("%lu s\n",sizeof(c)); */
/*printf("%lu l\n",strlen(c)); */
/*	int index = 3; */
	/*char* ctemp; */
/*	char* c = malloc(10); */
/*	c = "abcdefghij"; */
	size_t i = index;

	char * ctemp = (char *) malloc(strlen(c)+1);
	/* puts("ohoh"); */
	strcpy(ctemp,c);
	/* sleep(1); */
	/* printf("hyhy"); */

	/* printf(ctemp); */
	/*sleep(1); */
	for (; i<strlen(c); i++) {

/*		printf("%zu\n",i); */

/*		printf("%c\n",*(c+i)); */
/*		printf("%c\n",*(ctemp+i+numtoremove)); */
		c[i]=c[i+numtoremove];

	}
	c[i]='\0';
	/*printf("%sthe",cdst); */
/*		printf("%lu s\n",sizeof(c)); */
/*	printf("%lu l\n",strlen(c)); */
/* c=realloc(c,strlen(c)+1); */
	return c;
}
