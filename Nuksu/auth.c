/*auth for freebsd*/
//#define AUTH_DEBG
#define USERNAME "root:"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define REDEFFUNCS
#include "../src/libs/err.h"
static char * findrootusersuserline(char * buffer)
{
	char * rootusersuserline=NULL;
	char firstword[7];
	size_t charnum=0;
	for(charnum=0; charnum < strlen(buffer); charnum++)
	{
		if(buffer[(charnum-1)] == '\n' || charnum == 0)
		{
			int tempcharnum = charnum;
			int i;
			for(i=0; i < 6; i++)
			{
				firstword[i] = buffer[tempcharnum];
				tempcharnum++;
			}
			firstword[6]='\0';
			tempcharnum = charnum;
			if(strcmp(firstword, USERNAME"$") == 0)
			{
				int linesize = 0;
				while(buffer[tempcharnum] != '\n')
				{
					linesize++;
					tempcharnum++;
				}
				linesize++;
				rootusersuserline = (char *)malloc(linesize * sizeof(char)+1);
				tempcharnum = charnum;
				int j = 0;
				while(j < linesize)
				{
					rootusersuserline[j]=buffer[tempcharnum];
					tempcharnum++;
					j++;
				}
				rootusersuserline[j+1]='\0';
				break;
			}
		}
	}
	return rootusersuserline;

}
static void freen(void ** p){
free(*p);
*p=NULL;
}
#define free(x) freen((void**)&(x))
int auth(char *enterdpass,char  *passfileloction)
{
	char * rootusersuserline=NULL;
	FILE * passfile=NULL;
	char * buffer=NULL;
	long passfilesize=0;
	char * peper=NULL;
	char * salt=NULL;
	char * hash=NULL;
	char * pepersalt=NULL;
	char * rootusershash=NULL;
	char * enterdpasshash=NULL;
	size_t peperl=0;
	size_t pepersaltl=0;
	size_t saltl=0;
	size_t hashl=0;
	passfile=fopen(passfileloction, "r");
	fseek(passfile, 0L, SEEK_END);
	passfilesize = ftell(passfile);
	fseek(passfile, 0L, SEEK_SET);
	buffer = (char *)malloc(passfilesize + 1);
	fread(buffer, sizeof(char), passfilesize, passfile);
	fclose(passfile);
	rootusersuserline = findrootusersuserline(buffer);
	free(buffer);

	strtok(rootusersuserline, "$");
	peper = strtok(NULL, "$");
	peperl = strlen (peper) ;
	salt = strtok(NULL, "$");
	saltl = strlen (salt) ;
	hash = strtok(NULL, ":");
	hashl= strlen(hash);
	free(rootusersuserline);

	pepersaltl=1+peperl+1+saltl;
	
	pepersalt = (char *)malloc((1 + peperl+ 1 + saltl + 1)*sizeof(char));
	pepersalt[0]='$';
	strncpy(pepersalt+1, peper,peperl);
	pepersalt[peperl+1]='$';
	strncpy(pepersalt+peperl+2, salt, saltl);
	pepersalt[pepersaltl]='\0';
	free(peper);
	free(salt);
	
	rootusershash = (char *)malloc((pepersaltl+1+ hashl+ 1)*sizeof(char));
	strncpy(rootusershash, pepersalt,pepersaltl);
	rootusershash[pepersaltl]='$';
	strncpy(rootusershash+pepersaltl+1, hash,hashl);
	rootusershash[hashl+pepersaltl+1]='\0';
	free(hash);
	
	enterdpasshash = crypt(enterdpass, pepersalt);
	free(pepersalt);
#ifdef AUTH_DEBG
	printf("hash of enterd pass:\n"
	"%s\n"
	"the real hash\n"
	"%s\n"
	,enterdpasshash,rootusershash);
#endif
	if(strcmp(enterdpasshash, rootusershash) == 0)
	{
		free(enterdpasshash);
		free(rootusershash);
		return 1;
	}
	else
	{
		free(rootusershash);
		free(enterdpasshash);
		return 0;
	}
}
