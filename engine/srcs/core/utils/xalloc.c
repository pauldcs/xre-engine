#include "xre_compiler.h"
#include <stdlib.h>
#include <unistd.h>

void *xmalloc(size_t n)
{
	void *ptr;

	if (!(ptr = malloc((unsigned)n))) {
		write(STDERR_FILENO, "virtual memory exceeded\n", 24);
		exit(1);
	}
	return (ptr);
}

void *xrealloc(void *ptr, size_t n)
{
	__return_val_if_fail__(ptr, NULL);

	if ((ptr = realloc(ptr, (unsigned)n)) == NULL) {
		write(STDERR_FILENO, "virtual memory exceeded\n", 24);
		exit(1);
	}
	return (ptr);
}

void xfree(void *ptr)
{
	__return_if_fail__(ptr);
	free(ptr);
}
