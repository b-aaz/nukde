#include"err.h"
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<stdarg.h>
#include <string.h>
#define STRIFY(x) #x
#define NIFY(x) x
#define TOSTRIFY(x) STRIFY(x)

#define ERRCOLOR ""
#define ERRCOLORRE ""

static void errno_die(const char * line,const char * pfunc,const char * file,const char * fmt,...)
{
	va_list lastwords;
	fprintf(stderr,ERRCOLORRE"%s:%s:%s:%s:%s:%s: ",file,__DATE__,__TIME__,line,pfunc,strerror(errno));
	va_start(lastwords,fmt);
	vfprintf(stderr,fmt,lastwords);
	va_end(lastwords);
	fputs(ERRCOLORRE"\n",stderr);
	//fflush();
	exit(EXIT_FAILURE);
}

FILE * fopen_e(const char * path,const char * mode,const char * line,const char * fun,const char * file)
{
	FILE * re = fopen(path,mode);
	if(re==NULL)
	{
		errno_die(line,fun,file,"%s(%s,%s)","fopen",path,mode);
	}
	return re;
}
/*#ifdef REDEFFUNCS
#define fopen fopenE
#endif
#define fopenE(path, mode) fopen_e(path, mode , TOSTRIFY(__LINE__) , __func__ , __FILE__)
*/
void * malloc_e(size_t size,const char * line,const char * fun,const char * file)
{
	void * re = malloc(size);
	if(re==NULL)
	{
		errno_die(line,fun,file,"%s(%lu)","malloc",size);
	}
	return re;
}
/*
#ifdef REDEFFUNCS
#define malloc mallocE
#endif
#define mallocE(size) malloc_e(size, TOSTRIFY(__LINE__) , __func__ , __FILE__)
*/
