#include "array.h"
#include "xre_assert.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void sequence_drop(void *ptr)
{
	__return_if_fail__(ptr);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping sequence @%p", ptr);
#endif
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
		if (obj) {
			obj->repr(obj->data.ptr);
		} else {
			(void)fprintf(stderr, "<no_repr>");
		}

		if (i != seq_size - 1) {
			(void)fprintf(stderr, ", ");
		}
	}
	(void)fprintf(stderr, "]");
}

static bool unfold_sequence_object(object_t *object, array_t *buffer)
{
	__return_val_if_fail__(object, false);
	__return_val_if_fail__(buffer, false);

	if (__object_has_attr(object, ATTR_SEQUENCE)) {
		if (!array_concat(buffer, (array_t *)object->data.ptr)) {
			return (false);
		}

	} else {
		if (!array_append(buffer, object, 1)) {
			return (false);
		}
	}

	return (true);
}

object_t *object_sequence_create(size_t depth, object_t *lval, object_t *rval)
{
	__return_val_if_fail__(lval, NULL);
	__return_val_if_fail__(rval, NULL);

	static object_t object = { .repr = sequence_repr,
				   .drop = sequence_drop,
				   .is_true = sequence_test };

	array_t *sequence = array_create(sizeof(object_t), 2, object_drop);

	if (__object_get_depth(lval) > __object_get_depth(rval)) {
		if (!array_append(sequence, lval, 1) ||
		    !unfold_sequence_object(rval, sequence)) {
			goto prison;
		}
	} else {
		if (__object_get_depth(lval) != depth) {
			if (!array_append(sequence, lval, 1) ||
			    !array_append(sequence, rval, 1)) {
				goto prison;
			}
		} else if (!unfold_sequence_object(lval, sequence) ||
			   !array_append(sequence, rval, 1)) {
			goto prison;
		}
	}

	__object_set_attr(&object, ATTR_SEQUENCE);
	__object_set_data_ptr(&object, sequence);
	__object_set_data_size(&object, array_sizeof(sequence));
	__object_set_ref_count(&object, 0);
	__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created sequence @%p", object.data.ptr);
#endif
	return (&object);

prison:
	array_kill(sequence);
	return (NULL);
}
