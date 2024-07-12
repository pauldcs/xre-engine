#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_pow)
{
	return (set_current_error(self, XRE_NOT_IMPLEMENTED_ERROR), false);
}
