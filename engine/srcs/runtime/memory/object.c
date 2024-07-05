#include "xre_assert.h"
#include "xre_memory.h"
#include "xre_utils.h"
#include "xre_log.h"
#include "array.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

void object_flags_repr(int32_t flags)
{
	const char *separator = "";
	if (flags & FLAG_REGISTER) {
		fprintf(stderr, "register");
		separator = " | ";
	}
	if (flags & FLAG_ALLOC) {
		fprintf(stderr, "%salloc", separator);
		separator = " | ";
	}
	if (flags & FLAG_REFERENCE) {
		fprintf(stderr, "%sreference", separator);
		separator = " | ";
	}
	if (flags & FLAG_SEQUENCE) {
		fprintf(stderr, "%ssequence", separator);
		separator = " | ";
	}
	if (flags & FLAG_SYMBOL) {
		fprintf(stderr, "%ssymbol", separator);
	}
	fprintf(stderr, "\n");
}

static void print_number(void *ptr)
{
	int64_t num = (int64_t)ptr;
#if defined(__linux__)
	(void)fprintf(stderr, "%ld", num);
#else
	(void)fprintf(stderr, "%lld", num);
#endif
}

static void print_symbol(void *ptr)
{
	__return_if_fail__(ptr);

	(void)fprintf(stderr, "symbol@%p", ptr);
}

static void print_slice(void *ptr)
{
	__return_if_fail__(ptr);

	char *escaped = str_escape(ptr, strlen(ptr));
	(void)fprintf(stderr, "%s", (char *)escaped);
	free(escaped);
}

static void print_sequence(void *ptr)
{
	__return_if_fail__(ptr);

	array_t *array = (array_t *)ptr;
	size_t seq_size = array_size(array);

	(void)fprintf(stderr, "[");
	for (size_t i = 0; i < seq_size; i++) {
		object_t *obj = array_access(array, i);
		if (obj)
			obj->repr(obj->data.ptr);
		else
			(void)fprintf(stderr, "???");
		if (i != seq_size - 1) {
			(void)fprintf(stderr, ", ");
		}
	}
	(void)fprintf(stderr, "]");
}

static void dealloc_sequence(void *ptr)
{
	__return_if_fail__(ptr);

	array_kill((array_t *)ptr);
}

static void dealloc_none(void *ptr)
{
	(void)ptr;
}

object_t *object_create_register(int64_t data)
{
	static object_t object = { .flags = FLAG_REGISTER,
				   .repr = print_number,
				   .dealloc = dealloc_none,
				   .test = NULL };

	object.data.ptr = (void *)data;
	object.data.size = sizeof(int64_t);
	return (&object);
}

object_t *object_create_undefined(void)
{
	static object_t object = { .flags = FLAG_MUTABLE };
	object.data.ptr = NULL;
	object.data.size = 0;
	return (&object);
}

object_t *object_create_symbol(int64_t offset)
{
	__return_val_if_fail__(offset >= 0, NULL);

	static object_t object = { .flags = FLAG_SYMBOL,
				   .repr = print_symbol,
				   .dealloc = dealloc_none,
				   .test = NULL };

	object.data.ptr = (void *)offset;
	object.data.size = sizeof(int64_t);
	return (&object);
}

object_t *object_create_slice(unsigned char *ptr, size_t size)
{
	__return_val_if_fail__(ptr, NULL);

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

bool unwrap_sequence_object(object_t *object, array_t *buffer)
{
	__return_val_if_fail__(object, false);
	__return_val_if_fail__(buffer, false);

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

static void object_dealloc(void *ptr)
{
	((object_t *)ptr)->dealloc(((object_t *)ptr)->data.ptr);
}

object_t *object_create_sequence(object_t *lval, object_t *rval)
{
	__return_val_if_fail__(lval, NULL);
	__return_val_if_fail__(rval, NULL);

	static object_t object = { .flags = FLAG_ALLOC | FLAG_SEQUENCE,
				   .repr = print_sequence,
				   .dealloc = dealloc_sequence,
				   .test = NULL };

	array_t *sequence = array_create(sizeof(object_t), 2, object_dealloc);

	if (lval->depth > rval->depth) {
		if (!array_append(sequence, lval, 1) ||
		    !unwrap_sequence_object(rval, sequence)) {
			array_kill(sequence);
			return (NULL);
		}
	} else {
		if (!unwrap_sequence_object(lval, sequence) ||
		    !array_append(sequence, rval, 1)) {
			array_kill(sequence);
			return (NULL);
		}
	}

	object.data.ptr = sequence;
	object.data.size = array_sizeof(sequence);

	return (&object);
}

bool is_true_object(const object_t *object)
{
	__return_val_if_fail__(object, false);

	if ((int64_t)object->data.ptr) {
		return (true);
	}

	return (false);
}
