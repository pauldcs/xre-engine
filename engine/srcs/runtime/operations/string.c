#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>
#include <string.h>

XRE_API_OPERATOR_FUNC(oper_string)
{
	__return_val_if_fail__(self, false);

	if (!stack_push_enable_attrs(
		    self,
		    object_create_string((unsigned char *)self->string,
					 strlen(self->string)),
		    ATTR_READABLE)) {
		return (false);
	}

	return (true);
}
