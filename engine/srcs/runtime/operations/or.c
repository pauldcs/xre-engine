#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_or)
{
	__return_val_if_fail__(self, false);

	if (!__br_eval(__left_branch)) {
		return (false);
	}

	if (is_true_object(stack_top())) {
		return (true);
	}

	stack_pop_discard();

	return (__br_eval(__right_branch));
}
