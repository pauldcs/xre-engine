#include "xre_object.h"
#include "xre_assert.h"
#include "xre_memory.h"
#include "xre_log.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

static void print_number(void *ptr)
{
	int64_t num = (int64_t)ptr;
#if defined(__linux__)
	(void)fprintf(stderr, "%ld\n", num);
#else
	(void)fprintf(stderr, "%lld\n", num);
#endif
}

static void print_symbol(void *ptr)
{
	(void)fprintf(stderr, "symbol@%p\n", ptr);
}

static void print_slice(void *ptr)
{
	(void)fprintf(stderr, "%s\n", (char *)ptr);
}

object_t *object_create_register(int64_t data)
{
	static object_t object = { .flags = FLAG_REGISTER,
				   .repr = print_number,
				   .dealloc = NULL,
				   .test = NULL };

	object.data.ptr = (void *)data;
	return (&object);
}

object_t *object_create_symbol(int64_t offset)
{
	static object_t object = { .flags = FLAG_SYMBOL,
				   .repr = print_symbol,
				   .dealloc = NULL,
				   .test = NULL };

	object.data.ptr = (void *)offset;
	return (&object);
}

object_t *object_create_slice(unsigned char *ptr, size_t size)
{
	static object_t object = { .flags = FLAG_ALLOC,
				   .repr = print_slice,
				   .dealloc = free,
				   .test = NULL };

	object.data.ptr = malloc(size + 1);
	if (!object.data.ptr) {
		return (NULL);
	}

	object.data.size = size;
	(void)strcpy(object.data.ptr, (char *)ptr);
	return (&object);
}

bool is_true_object(object_t *object)
{
	__return_val_if_fail__(object, false);

	if ((int64_t)object->data.ptr) {
		return (true);
	}

	return (false);
}
