#ifndef NUERR
#define NUERR
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define STRIFY(x) #x
#define NIFY(x) x
#define TOSTRIFY(x) STRIFY(x)

#ifndef NUERRCOLOR
	#define NUERRCOLOR ""
#endif
#ifndef NUERRCOLORRE
	#define NUERRCOLORRE ""
#endif
#ifndef CUSTOMMSG
#	define CUSTOMMSG NULL
#endif
static void die_ve (const char * line, const char * pfunc, const char * file, const char * cusmsg,const char * fmt, va_list lastwords) {
	
	fprintf (stderr, NUERRCOLOR "%s:%s:%s:%s:%s ", file, __DATE__, __TIME__, line, pfunc);
	if (cusmsg!=NULL){fprintf (stderr, ":%s:",cusmsg);}
	vfprintf (stderr, fmt, lastwords);
	fputs (NUERRCOLORRE "\n", stderr);
	exit (EXIT_FAILURE);
}

static void errno_die (const char * line, const char * pfunc, const char * file,const char * cusmsg, const char * fmt, ...)
{
	fprintf (stderr, NUERRCOLOR "%s: " NUERRCOLORRE, strerror (errno));
	va_list lastwords;
	va_start (lastwords, fmt);
	die_ve (line, pfunc, file ,cusmsg,fmt, lastwords);
	va_end (lastwords);
}

static void die_e (const char * line, const char * pfunc, const char * file,const char * cusmsg, const char * fmt, ...)
{
	va_list lastwords;
	va_start (lastwords, fmt);
	die_ve (line, pfunc, file ,cusmsg,fmt, lastwords);
	va_end (lastwords);
}

#ifdef NUERRREDEFFUNCS
	#define die dieE
#endif
#define dieE(fmt, ...)                                                         \
	die_e(TOSTRIFY(__LINE__), __func__, __FILE__,CUSTOMMSG, fmt, __VA_ARGS__)

#ifdef NUERRSTDIO
static FILE * fopen_e (const char * path, const char * mode, const char * line, const char * fun, const char * file,const char * cusmsg)
{
	FILE * re = fopen (path, mode);

	if (re == NULL)
	{
		errno_die (line, fun, file, cusmsg, "%s(%s,%s)", "fopen", path, mode);
	}

	return re;
}
#ifdef NUERRREDEFFUNCS
	#define fopen fopenE
#endif
#  define fopenE(path, mode)                                                   \
	fopen_e(path, mode, TOSTRIFY(__LINE__), __func__, __FILE__,CUSTOMMSG)
#endif

#ifdef NUERRSTDLIB
static void * malloc_e (size_t size, const char * line, const char * fun, const char * file,const char * cusmsg)
{
	void * re = malloc (size);

	if (re == NULL)
	{
		errno_die (line, fun, file, cusmsg, "%s(%lu)", "malloc", size);
	}

	return re;
}
#ifdef NUERRREDEFFUNCS
	#define malloc mallocE
#endif
#  define mallocE(size) malloc_e(size, TOSTRIFY(__LINE__), __func__, __FILE__,CUSTOMMSG)
#endif

#ifdef NUERRSYSEVENT
int kqueue_e (const char * line, const char * pfunc, const char * file,const char * cusmsg)
{
	int kqid;
	kqid = kqueue();

	if (kqid == -1)
	{
		die_ve (line, pfunc, file ,cusmsg,"%s\n", "kqueue initialization failure");
	}

	return kqid;
}
#ifdef NUERRREDEFFUNCS
	#define kqueue kqueueE
#endif
#  define kqueueE() kqueue_e(TOSTRIFY(__LINE__), __func__, __FILE__,CUSTOMMSG)
#endif

#ifdef NUERRLIBMAGIC
magic_t magic_open_e (int flags, const char * line, const char * pfunc, const char * file,const char * cusmsg)
{
	magic_t magic_cookie;
	magic_cookie = magic_open (flags);

	if (magic_cookie == NULL)
	{
		die_e (line, pfunc, file ,cusmsg,"%s : magic(%d)\n",
			   "Unable to initialize magic library", flags);
	}

	return magic_cookie;
}
void magic_load_e (magic_t cookie, const char * path, const char * line, const char * pfunc, const char * file,const char * cusmsg)
{
	if (magic_load (cookie, path) != 0)
	{
		magic_error (cookie);
		magic_close (cookie);
		die_e (line, pfunc, file ,cusmsg,"%s\n", "Unable to load magic database");
	}
}

#ifdef NUERRREDEFFUNCS
	#define magic_open magic_openE
	#define magic_load magic_loadE
#endif
#  define magic_openE(flags)                                                    \
	magic_open_e(flags, TOSTRIFY(__LINE__), __func__, __FILE__,CUSTOMMSG)
#  define magic_loadE(cookie,path)                                                    \
	magic_load_e(cookie,path,TOSTRIFY(__LINE__), __func__, __FILE__,CUSTOMMSG)

#endif

#endif
