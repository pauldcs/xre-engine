#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_inject)
{
	return (trigger_error_on(self, XRE_NOT_IMPLEMENTED_ERROR), false);
}
