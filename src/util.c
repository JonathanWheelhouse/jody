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

void * xalloc(int size)
{
	 void *p = malloc(size);
	 if (p == NULL)
		  fatal("out of memory");
	 return p;
}


/* This is a simple custom pseudorandom number generator. It's not a very
   good one, but it's sufficient for our purposes. Never trust the rand()
   included with the C library. Its quality varies between implementations,
   and it's easy to run into patterns within the generated numbers. At least
   this one is consistent. */
static Sint32 seed = 0;

void initrandom(void)
{
	 seed = time(NULL);
}

