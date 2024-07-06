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

	(void)ptr;
}

object_t *object_create_symbol(int64_t offset)
{
	__return_val_if_fail__(offset >= 0, NULL);

	static object_t object = { .attrs = ATTR_SYMBOL,
				   .repr = symbol_repr,
				   .drop = symbol_drop,
				   .is_true = NULL };

	object.data.ptr = (void *)offset;
	object.data.size = sizeof(int64_t);

	return (&object);
}

inline static bool expand_symbol(ast_stmt_t *self, int64_t offset,
				 object_t **object)
{
	*object = array_access(g_symtab, offset);
	if (!object) {
		return (set_current_error(self, XRE_OUT_OF_BOUNDS_ACCESS_ERROR),
			false);
	}
	return (true);
}

bool expand_symbol_read(ast_stmt_t *self, int64_t offset, object_t **object)
{
	if (!expand_symbol(self, offset, object)) {
		return (false);
	}
	if ((*object)->attrs & ATTR_READABLE) {
		return (true);
	}

	return (set_current_error(self, XRE_UNREADABLE_ERROR), false);
}

bool expand_symbol_write(ast_stmt_t *self, int64_t offset, object_t **object)
{
	if (!expand_symbol(self, offset, object)) {
		return (false);
	}

	if (!((*object)->attrs & ATTR_MUTABLE)) {
		return (set_current_error(self, XRE_WRITE_ON_READONLY_ERROR),
			false);
	}
	return (true);
}
