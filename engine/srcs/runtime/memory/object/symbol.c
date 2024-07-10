#include "xre_assert.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void symbol_repr(void *ptr)
{
	__return_if_fail__(ptr);

	(void)fprintf(stderr, "symbol@%p", ptr);
}

static void symbol_drop(void *ptr)
{
	__return_if_fail__(ptr);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping symbol @%p", ptr);
#endif
}

object_t *object_symbol_create(int64_t offset)
{
	__return_val_if_fail__(offset >= 0, NULL);

	static object_t object = { .repr = symbol_repr,
				   .drop = symbol_drop,
				   .is_true = NULL };

	__object_set_attr(&object, ATTR_SYMBOL);
	__object_set_data_ptr(&object, offset);
	__object_set_data_size(&object, sizeof(int64_t));
	__object_set_ref_count(&object, 0);
	//__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created symbol @%p", object.data.ptr);
#endif
	return (&object);
}

inline static bool unwrap_symbol(ast_stmt_t *self, int64_t offset,
				 object_t **object)
{
	*object = array_access(g_symtab, offset);
	if (!object) {
		return (set_current_error(self, XRE_OUT_OF_BOUNDS_ACCESS_ERROR),
			false);
	}
	return (true);
}

bool unwrap_symbol_read(ast_stmt_t *self, int64_t offset, object_t **object)
{
	if (!unwrap_symbol(self, offset, object)) {
		return (false);
	}

	if (__object_has_attr(*object, ATTR_READABLE)) {
		return (true);
	}

	return (set_current_error(self, XRE_UNREADABLE_ERROR), false);
}

bool unwrap_symbol_write(ast_stmt_t *self, int64_t offset, object_t **object)
{
	if (!unwrap_symbol(self, offset, object)) {
		return (false);
	}

	if (!__object_has_attr(*object, ATTR_MUTABLE)) {
		return (set_current_error(self, XRE_WRITE_ON_READONLY_ERROR),
			false);
	}
	return (true);
}
