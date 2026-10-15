#pragma once

#include <stddef.h>
#include <stdlib.h>

/* GCC / Clang attributes */
#if defined(__GNUC__) || defined(__clang__)
#  define ATTRIB_PRINTF(fmt_idx, arg_idx) __attribute__((format(printf, fmt_idx, arg_idx)))
#  define ATTRIB_MALLOC __attribute__((malloc))
#  define ATTRIB_ALLOC_SIZE(x) __attribute__((alloc_size(x)))
#  define ATTRIB_ALLOC_SIZE2(x, y) __attribute__((alloc_size(x, y)))
#  define ATTRIB_WARN_UNUSED __attribute__((warn_unused_result))
#else
#  define ATTRIB_PRINTF(fmt_idx, arg_idx)
#  define ATTRIB_MALLOC
#  define ATTRIB_ALLOC_SIZE(x)
#  define ATTRIB_ALLOC_SIZE2(x, y)
#  define ATTRIB_WARN_UNUSED
#endif

/* C23 [[noreturn]] combined with GCC format checking */
[[noreturn]] void fatal(const char *format, ...) ATTRIB_PRINTF(1, 2);

/* Type-safe inline functions instead of side-effect prone macros */
static inline int max_int(int a, int b) { return (a > b) ? a : b; }
static inline int min_int(int a, int b) { return (a < b) ? a : b; }
static inline int rand_range(int lb, int ub) { return (rand() % ((ub) - (lb) + 1)) + (lb); }

ATTRIB_WARN_UNUSED char *jody_image_path(const char *filename);

ATTRIB_MALLOC ATTRIB_ALLOC_SIZE(1) ATTRIB_WARN_UNUSED
void *xmalloc(size_t size);

ATTRIB_MALLOC ATTRIB_ALLOC_SIZE2(1, 2) ATTRIB_WARN_UNUSED
void *xcalloc(size_t nmemb, size_t size);
