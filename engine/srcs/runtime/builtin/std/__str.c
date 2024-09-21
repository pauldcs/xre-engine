#include "xre_compiler.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_str)
{
	__return_val_if_fail__(self, false);

	static object_t *top;
        static object_t buffer = { 0};

	if (!__br_eval(__left_branch)) {
		return (false);
	}

	top = (object_t *)stack_top();
        object_attrs_repr(__object_get_attr(top));
	object_buffer_init(top, &buffer);
        return (__push_r(
		self, &buffer
	));
}
