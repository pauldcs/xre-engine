#include "xre_builtin.h"
#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_typeof)
{
	__return_val_if_fail__(self, false);

	static object_t *top;

	if (!BR_EVAL((LEFT_BRANCH))) {
		return (false);
	}

	top = (object_t *)stack_top();
	flags_to_string(top->flags);

	return (true);
}
