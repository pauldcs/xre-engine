#include "vec.h"
#include "xre_compiler.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void buffer_drop(void *ptr)
{
	__return_if_fail__(ptr);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping sequence @%p", ptr);
#endif
	vec_kill((vec_t *)ptr);
}

static bool buffer_test(void *ptr)
{
	__return_val_if_fail__(ptr, false);

	return (vec_size((vec_t *)ptr) > 0);
}

static void buffer_repr(void *ptr)
{
	__return_if_fail__(ptr);

	vec_t *vec	= (vec_t *)ptr;
	size_t seq_size = vec_size(vec);

	for (size_t i = 0; i < seq_size; i++) {
		char ch = *(char *)vec_access(vec, i);
                (void)fprintf(stdout, "%02x", ch);
	}
	(void)fprintf(stdout, "\n");
}

bool object_buffer_init(object_t *source, object_t *buf)
{
	__return_val_if_fail__(source, NULL);
	__return_val_if_fail__(buf, NULL);

	buf->repr    = buffer_repr;
	buf->drop    = buffer_drop;
	buf->is_true = buffer_test;

	xre_buffer_t *buffer =
		vec_create(sizeof(char), 64, NULL);

	__object_set_attr(buf, ATTR_BUFFER);
	__object_set_data_as_buffer(buf, buffer);
	__object_set_ref_count(buf, 0);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created sequence @%p", object.data.ptr);
#endif
	return (true);

}
