#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_not)
{
	__return_val_if_fail__(self, false);

	static object_t v;

	if (!__br_eval(__left_branch) ||
	    !stack_pop_readable(&v, __left_branch)) {
		return (false);
	}

	return (stack_push_enable_attrs(
		self, object_create_register(!is_true_object(&v)),
		ATTR_READABLE | ATTR_MUTABLE));
}
