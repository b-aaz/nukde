#include <stdlib.h>
#include <string.h>
char * rmchrbyidx(char * c,int index,int numtoremove)
{
	size_t i = index;
	char * ctemp = (char *) malloc(strlen(c)+1);
	strcpy(ctemp,c);
	for(; i<strlen(c); i++)
	{
		c[i]=c[i+numtoremove];
	}
	c[i]='\0';
	return c;
}
