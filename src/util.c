#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

#include "datadir.h"
#include "util.h"


const char * jody_image_path(const char *filename)
{
    static char path[4096];

    /*
     * Prefer the source tree while developing.
     */
    snprintf(
        path,
        sizeof(path),
        "%s/%s",
        SOURCE_IMG_DIR,
        filename
    );

    FILE *file = fopen(path, "rb");
    if (file) {
        fclose(file);
        return path;
    }

    /*
     * Fall back to the installed location.
     */
    snprintf(
        path,
        sizeof(path),
        "%s/%s",
        INSTALL_IMG_DIR,
        filename
    );

    return path;
}


[[noreturn]] void fatal(const char *format, ...)
{
	 va_list args;

	 va_start(args, format);
	 vfprintf(stderr, format, args);
	 va_end(args);
	 fprintf(stderr, "\n");
	 exit(EXIT_FAILURE);
}

void * xmalloc(size_t size)
{
	 void *p = malloc(size);
	 if (p == nullptr)
		  fatal("Out of memory");
	 return p;
}


void * xcalloc(size_t nmemb, size_t size)
{
	void *p = calloc(nmemb, size);
	if (p == nullptr)
		fatal("Out of memory");
	return p;
}

static Sint32 seed = 0;

void initrandom(void)
{
	 seed = time(nullptr);
}
