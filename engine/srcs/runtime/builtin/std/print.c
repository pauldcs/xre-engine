#include "xre_assert.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_print)
{
	__return_val_if_fail__(self, false);

	static object_t *top;

	if (!__br_eval(__left_branch)) {
		return (false);
	}

	top = (object_t *)stack_top();
	top->repr(__object_get_data_as_any(top));

	(void)fprintf(stderr, "\n");

	return (true);
}
