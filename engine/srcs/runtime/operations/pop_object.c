#include "xre_operations.h"
#include "xre_object.h"
#include "xre_memory.h"
#include <stdbool.h>

bool pop_object(object_t *ptr, ast_stmt_t *stmts)
{
	stack_pop(ptr);

	if (!(ptr->flags & FLAG_READABLE)) {
		return (trigger_error_on(stmts, XRE_UNREADABLE_ERROR), false);
	}

	return (true);
}
