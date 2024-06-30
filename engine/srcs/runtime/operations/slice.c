#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>
#include <string.h>

XRE_OPERATOR_API(oper_string)
{
	__return_val_if_fail__(self, false);

	if (!stack_push(object_create_slice((unsigned char *)self->string,
					    strlen(self->string)))) {
		return (set_error_type(XRE_STACK_OVERFLOW_ERROR),
			set_error_orig(self), false);
	}

	return (true);
}