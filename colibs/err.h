#ifndef NUERR
#define NUERR
#include<stdio.h>
#include <stdarg.h>
#include<errno.h>
#include<string.h>
#define STRIFY(x) #x
#define NIFY(x) x
#define TOSTRIFY(x) STRIFY(x)

#ifndef NUERRCOLOR 
#define NUERRCOLOR ""
#endif
#ifndef NUERRCOLORRE
#define NUERRCOLORRE ""
#endif
static void die_ve(const char * line,const char * pfunc,const char * file,const char * fmt,va_list lastwords)
{
	fprintf(stderr,NUERRCOLOR"%s:%s:%s:%s:%s ",file,__DATE__,__TIME__,line,pfunc);
	vfprintf(stderr,fmt,lastwords);
	fputs(NUERRCOLORRE"\n",stderr);
	exit(EXIT_FAILURE);
}

static void errno_die(const char * line,const char * pfunc,const char * file,const char * fmt,...)
{
	fprintf(stderr,NUERRCOLOR"%s: "NUERRCOLORRE , strerror(errno));
	va_list lastwords;
	va_start(lastwords,fmt);
	die_ve(line,pfunc,file,fmt,lastwords);
	va_end(lastwords);
}



static void die_e(const char * line,const char * pfunc,const char * file,const char * fmt,...)
{
	va_list lastwords;
	va_start(lastwords,fmt);
	die_ve(line,pfunc,file,fmt,lastwords);
	va_end(lastwords);
}

#ifdef NUERRREDEFFUNCS
#define die dieE
#endif
#define dieE(fmt,...) die_e(TOSTRIFY(__LINE__) , __func__ , __FILE__,fmt,__VA_ARGS__)

#ifdef NUERRSTDIO 
static FILE * fopen_e(const char * path,const char * mode,const char * line,const char * fun,const char * file)
{
	FILE * re = fopen(path,mode);
	if(re==NULL)
	{
		errno_die(line,fun,file,"%s(%s,%s)","fopen",path,mode);
	}
	return re;
}
#ifdef NUERRREDEFFUNCS
#define fopen fopenE
#endif
#define fopenE(path, mode) fopen_e(path, mode , TOSTRIFY(__LINE__) , __func__ , __FILE__)
#endif

#ifdef NUERRSTDLIB
static void * malloc_e(size_t size,const char * line,const char * fun,const char * file)
{
	void * re = malloc(size);
	if(re==NULL)
	{
		errno_die(line,fun,file,"%s(%lu)","malloc",size);
	}
	return re;
}
#ifdef NUERRREDEFFUNCS
#define malloc mallocE
#endif
#define mallocE(size) malloc_e(size , TOSTRIFY(__LINE__) , __func__ , __FILE__)
#endif 


#endif 


