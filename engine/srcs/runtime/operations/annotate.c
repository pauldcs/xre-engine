#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_annotate)
{
	XRE_LOGGER(debug, "oper_annotate");
	return (set_error_type(XRE_NOT_IMPLEMENTED_ERROR), set_error_orig(self),
		false);
}
