#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "auth.h"

char * findrootline(char * buffer)
{
	char * rootline;
	char firstword[7];
	size_t charnum;
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
			puts(firstword);
			tempcharnum = charnum;
			if(strcmp(firstword, "root:$") == 0)
			{
				int linesize = 0;
				while(buffer[tempcharnum] != '\n')
				{
					linesize++;
					tempcharnum++;
				}
				linesize++;
				rootline = (char *)malloc(linesize * sizeof(char)+1);
				tempcharnum = charnum;
				int j = 0;
				while(j < linesize)
				{
					rootline[j]=buffer[tempcharnum];
					tempcharnum++;
					j++;
				}
				rootline[j+1]='\0';
				break;
			}
		}
	}
	return rootline;
}
int auth(char enterdpass[],char  passfileloction[])
{
	char * username = NULL;
	char * peper = NULL;
	char * hash = NULL;
	char * salt = NULL;
	char * rootline = NULL;
	char * pepersalt = NULL;
	char * rootlinewoj = NULL;
	char * enterdpasshash = NULL;
	char * enterdpasshashwun = NULL;
	FILE * passfile = fopen(passfileloction, "r");
	char * buffer;
	long passfilesize;
	const char s[2] = "$";
	fseek(passfile, 0L, SEEK_END);
	passfilesize = ftell(passfile);
	fseek(passfile, 0L, SEEK_SET);
	buffer = (char *)malloc(passfilesize + 1);
	fread(buffer, sizeof(char), passfilesize, passfile);
	fclose(passfile);
	rootline = findrootline(buffer);
	free(buffer);
	buffer=NULL;
	username = strtok(rootline, s);
	peper = strtok(NULL, s);
	salt = strtok(NULL, s);
	hash = strtok(NULL, ":");
	rootlinewoj = (char *)malloc((1 + strlen(peper) + 1 + strlen(salt) + strlen(username) + 1 + strlen(hash) + 1)*sizeof(char));
	pepersalt = (char *)malloc((1 + strlen(peper)+ 1 + strlen(salt) + 1)*sizeof(char));
	strcat(pepersalt, s);
	strcat(pepersalt, peper);
	strcat(pepersalt, s);
	strcat(pepersalt, salt);
	strcat(rootlinewoj, username);
	strcat(rootlinewoj, pepersalt);
	strcat(rootlinewoj, s);
	strcat(rootlinewoj, hash);
	enterdpasshash = crypt(enterdpass, pepersalt);
	free(pepersalt);
	pepersalt=NULL;
	enterdpasshashwun =
	    (char *)malloc((strlen(username) + strlen(enterdpasshash)+1)*sizeof(char));
	free(rootline);
	rootline=NULL;
	strcat(enterdpasshashwun, username);
	strcat(enterdpasshashwun, enterdpasshash);
	if(strcmp(enterdpasshashwun, rootlinewoj) == 0)
	{
		free(rootlinewoj);
		rootlinewoj=NULL;
		free(enterdpasshashwun);
		enterdpasshashwun=NULL;
		return 1;
	}
	else
	{
		free(rootlinewoj);
		rootlinewoj=NULL;
		free(enterdpasshashwun);
		enterdpasshashwun=NULL;
		return 0;
	}
}
