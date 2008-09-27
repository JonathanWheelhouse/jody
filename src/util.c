#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

void fatal(const char *format, ...)
{
	 va_list args;

	 va_start(args, format);
	 vfprintf(stderr, format, args);
	 va_end(args);
	 fprintf(stderr, "\n");
	 exit(1);
}

void * xmalloc(size_t size)
{
	 void *p = malloc(size);
	 if (p == NULL)
		  fatal("Out of memory");
	 return p;
}


void * xcalloc(size_t nmemb, size_t size)
{
	void *p = calloc(nmemb, size);
	if (p == NULL)
		fatal("Out of memory");
	return p;
}

static Sint32 seed = 0;

void initrandom(void)
{
	 seed = time(NULL);
}

