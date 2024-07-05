#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_separator)
{
	__return_val_if_fail__(self, false);

	if (!__br_eval(__left_branch)) {
		return (false);
	}

	stack_pop_discard();

	if (!__br_eval(__right_branch)) {
		return (false);
	}

	__stack_top_disable_flags(FLAG_READABLE);
	return (true);
}
