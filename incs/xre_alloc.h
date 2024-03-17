#ifndef __XRE_ALLOC_H__
#define __XRE_ALLOC_H__

#include "xre_core.h"
#include <stddef.h>
#include <stdlib.h>

#define XRE_MALLOC_WRAPPER 0
#define XRE_MALLOC_GLOBAL 0
#define XRE_NO_XMALLOC 0

typedef void *(t_xre_malloc)(size_t);
typedef void *(t_xre_realloc)(void *, size_t);
typedef void(t_xre_free)(void *);

#if XRE_MALLOC_WRAPPER

void xre_set_alloc_hooks(t_xre_malloc m, t_xre_realloc r, t_xre_free f);

#if XRE_MALLOC_GLOBAL

t_xre_malloc *xre_malloc;
t_xre_realloc *xre_calloc;
t_xre_free *xre_realloc;

#else
void *xre_malloc(size_t sz);
void *xre_calloc(size_t count, size_t sz);
void *xre_realloc(void *p, size_t sz);
void xre_free(void *p);
#endif

#else

void *xmalloc(size_t n);
void *xrealloc(void *ptr, size_t n);
void xfree(void *ptr);

#define xre_malloc(x) xmalloc((x))
#define xre_realloc(x, y) xrealloc((x), (y))
#define xre_free(x) xfree((x))

#if XRE_NO_XMALLOC
#define xmalloc(x) malloc((x))
#define xrealloc(x, y) realloc((x), (y))
#define xfree(x) free((x))
#endif
#endif

#endif /* __XRE_ALLOC_H__ */
