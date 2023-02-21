#ifndef NUERR
#define NUERR
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



FILE * fopen_e(const char * path,const char * mode,const char * line,const char * fun,const char * file);

#ifdef REDEFFUNCS
#define fopen fopenE
#endif
#define fopenE(path, mode) fopen_e(path, mode , TOSTRIFY(__LINE__) , __func__ , __FILE__)

void * malloc_e(size_t size,const char * line,const char * fun,const char * file);

#ifdef REDEFFUNCS
#define malloc mallocE
#endif
#define mallocE(size) malloc_e(size , TOSTRIFY(__LINE__) , __func__ , __FILE__)
#endif 
