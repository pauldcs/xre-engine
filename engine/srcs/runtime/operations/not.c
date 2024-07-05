#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_not)
{
	__return_val_if_fail__(self, false);

	static object_t v;

	if (!__br_eval(__left_branch) || !pop_object(&v, __left_branch)) {
		return (false);
	}

	return (stack_push_flagged(self,
				   object_create_register(!is_true_object(&v)),
				   FLAG_READABLE | FLAG_MUTABLE));
}
