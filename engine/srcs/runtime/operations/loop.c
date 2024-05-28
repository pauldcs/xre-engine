#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_OPERATOR_API(oper_loop)
{
	__return_val_if_fail__(self, false);

loop:
	if (!BR_EVAL((LEFT_BRANCH))) {
		return (false);
	}

	if (!ECHO_DOLLAR_QUESTION) {
		return (true);
	}

	if (!BR_EVAL((RIGHT_BRANCH))) {
		return (false);
	}

	goto loop;
}
