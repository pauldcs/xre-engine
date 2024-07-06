#include "array.h"
#include "xre_assert.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void sequence_drop(void *ptr)
{
	__return_if_fail__(ptr);

	array_kill((array_t *)ptr);
}

static bool sequence_test(void *ptr)
{
	__return_val_if_fail__(ptr, false);

	return (array_size((array_t *)ptr) > 0);
}

static void sequence_repr(void *ptr)
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

bool unwrap_sequence_object(object_t *object, array_t *buffer)
{
	__return_val_if_fail__(object, false);
	__return_val_if_fail__(buffer, false);

	if (object->attrs & ATTR_SEQUENCE) {
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

object_t *object_create_sequence(object_t *lval, object_t *rval)
{
	__return_val_if_fail__(lval, NULL);
	__return_val_if_fail__(rval, NULL);

	static object_t object = { .attrs = ATTR_SEQUENCE,
				   .repr = sequence_repr,
				   .drop = sequence_drop,
				   .is_true = sequence_test };

	array_t *sequence = array_create(sizeof(object_t), 2, object_drop);

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
