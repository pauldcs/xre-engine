#include "vec.h"
#include "xre_compiler.h"
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
	vec_kill((vec_t *)ptr);
}

static bool sequence_test(void *ptr)
{
	__return_val_if_fail__(ptr, false);

	return (vec_size((vec_t *)ptr) > 0);
}

static void sequence_repr(void *ptr)
{
	__return_if_fail__(ptr);

	vec_t *array	= (vec_t *)ptr;
	size_t seq_size = vec_size(array);

	(void)fprintf(stdout, "[");
	for (size_t i = 0; i < seq_size; i++) {
		object_t *obj = vec_access(array, i);
		if (obj) {
			obj->repr(__object_get_data_as_any(obj));
		} else {
			(void)fprintf(stdout, "<no_repr>");
		}

		if (i != seq_size - 1) {
			(void)fprintf(stdout, ", ");
		}
	}
	(void)fprintf(stdout, "]");
}

static bool unfold_sequence_object(object_t *object, vec_t *buffer)
{
	__return_val_if_fail__(object, false);
	__return_val_if_fail__(buffer, false);

	if (__object_has_attr(object, ATTR_SEQUENCE)) {
		if (!vec_concat(
			    buffer,
			    __object_get_data_as_sequence(object)
		    )) {
			return (false);
		}
		
		if (!__object_has_attr(object, ATTR_REFERENCE) && __object_get_depth(object) > 1) {
			sequence_drop(object->sequence);
		}

	} else {
		if (!vec_append(buffer, object, 1)) {
			return (false);
		}
	}

	return (true);
}

bool object_sequence_init(
	size_t depth, object_t *lval, object_t *rval, object_t *buf
)
{
	__return_val_if_fail__(lval, NULL);
	__return_val_if_fail__(rval, NULL);

	buf->repr    = sequence_repr;
	buf->drop    = sequence_drop;
	buf->is_true = sequence_test;

	xre_sequence_t *sequence =
		vec_create(sizeof(object_t), 2, object_drop);

	if (__object_get_depth(lval) > __object_get_depth(rval)) {
		if (!vec_append(sequence, lval, 1) ||
		    !unfold_sequence_object(rval, sequence)) {
			goto prison;
		}
	} else {
		if (__object_get_depth(lval) != depth) {
			if (!vec_append(sequence, lval, 1) ||
			    !vec_append(sequence, rval, 1)) {
				goto prison;
			}
		} else if (!unfold_sequence_object(lval, sequence) || !vec_append(sequence, rval, 1)) {
			goto prison;
		}
	}

	__object_set_attr(buf, ATTR_SEQUENCE);
	__object_set_data_as_sequence(buf, sequence);
	__object_set_ref_count(buf, 0);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created sequence @%p", object.data.ptr);
#endif
	return (true);

prison:
	vec_kill(sequence);
	return (false);
}
