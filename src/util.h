#pragma once

#include <stddef.h>
#include <stdlib.h>

#define UNUSED(x) (void)(x)

/* Type-safe inline functions instead of side-effect prone macros */
static inline int max_int(int a, int b) { return (a > b) ? a : b; }
static inline int min_int(int a, int b) { return (a < b) ? a : b; }
static inline int rand_range(int lb, int ub) { return (rand() % ((ub) - (lb) + 1)) + (lb); }

const char *jody_image_path(const char *filename);

void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
