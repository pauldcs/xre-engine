#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_OPERATOR_API(oper_separator)
{
	__return_val_if_fail__(self, false);

	static object_t v;

	if (!BR_EVAL((LEFT_BRANCH))) {
		return (false);
	}

	stack_pop(&v);

	if (!BR_EVAL((RIGHT_BRANCH))) {
		return (false);
	}

	return (true);
}