#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_OPERATOR_API(oper_and)
{
	__return_val_if_fail__(self, false);

	if (!BR_EVAL((LEFT_BRANCH))) {
		return (false);
	}

	if (!IS_ERROR_RETURN) {
		return (true);
	}

	if (!BR_EVAL((RIGHT_BRANCH))) {
		return (false);
	}

	return (true);
}
