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
	//vec_kill((vec_t *)ptr);
}

static bool sequence_test(void *ptr)
{
	__return_val_if_fail__(ptr, false);

	return (vec_size((vec_t *)ptr) > 0);
}

static void sequence_repr(void *ptr)
{
	__return_if_fail__(ptr);

	object_t *object;
	vec_t	 *array	   = (vec_t *)ptr;
	size_t	  seq_size = vec_size(array);

	(void)fprintf(stdout, "[");
	while (seq_size--) {
		object = (object_t *)vec_access(array, seq_size);
		object->repr(__object_get_data_as_any(object));
		fflush(stdout);

		if (seq_size) {
			(void)fprintf(stdout, ", ");
		}
	}
	(void)fprintf(stdout, "]");
}

bool object_sequence_init(vec_t *sequence, object_t *buf)
{
	__return_val_if_fail__(sequence, NULL);

	buf->repr    = sequence_repr;
	buf->drop    = sequence_drop;
	buf->is_true = sequence_test;

	__object_set_attr(buf, ATTR_SEQUENCE);
	__object_set_data_as_sequence(buf, sequence);
	__object_set_ref_count(buf, 0);
	//__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created string @%p", object.data.ptr);
#endif
	return (true);
}
