#include "xre_object.h"
#include "xre_assert.h"
#include "xre_memory.h"
#include "xre_log.h"
#include "array.h"
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

static void print_sequence(void *ptr)
{
	array_t *array = (array_t *)ptr;
	size_t seq_size = array_size(array);

	for (size_t i = 0; i < seq_size; i++) {
		object_t *obj = array_access(array, i);
		if (obj)
			obj->repr(obj->data.ptr);
		else 
			(void)fprintf(stderr, "???\n");
	}

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

bool unwrap_object(object_t *object, array_t *buffer) {
	if (object->flags & FLAG_SEQUENCE) {
		array_t *other = (array_t *)object->data.ptr;
		if (!array_concat(buffer, other)) {
			return (false);
		}

	} else {
		if (!array_append(buffer, object, 1)) {
			return (false);
		}
	}
	return (true);
}

static void dealloc_requence(void *ptr) {
	array_kill((array_t *)ptr);
}

object_t *object_create_sequence(object_t *lval, object_t *rval)
{
	static object_t object = { .flags = FLAG_ALLOC | FLAG_SEQUENCE,
				   .repr = print_sequence,
				   .dealloc = dealloc_requence,
				   .test = NULL };

	array_t *sequence = array_create(sizeof(object_t), 2, NULL);

	if (!unwrap_object(lval, sequence)
		|| !unwrap_object(rval, sequence)) {
			array_kill(sequence);
		return (NULL);
	}

	object.data.ptr = sequence;
	object.data.size = array_sizeof(sequence);

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
