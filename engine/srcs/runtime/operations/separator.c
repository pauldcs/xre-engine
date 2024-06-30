#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_separator)
{
	__return_val_if_fail__(self, false);

	if (!BR_EVAL((LEFT_BRANCH))) {
		return (false);
	}

	stack_pop_discard();

	if (!BR_EVAL((RIGHT_BRANCH))) {
		return (false);
	}

	STACK_TOP_DISABLE_FLAGS(FLAG_READABLE);
	return (true);
}
