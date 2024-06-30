#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_not)
{
	__return_val_if_fail__(self, false);

	static object_t v;

	if (!BR_EVAL((LEFT_BRANCH)) || !pop_object(&v, LEFT_BRANCH)) {
		return (false);
	}

	return (stack_push_flagged(self,
				   object_create_register(!is_true_object(&v)),
				   FLAG_READABLE | FLAG_MUTABLE));
}
