#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>
#include <string.h>

XRE_API_OPERATOR_FUNC(oper_string)
{
	__return_val_if_fail__(self, false);

	if (!stack_push_flagged(
		    self,
		    object_create_slice((unsigned char *)self->string,
					strlen(self->string)),
		    FLAG_READABLE)) {
		return (false);
	}

	return (true);
}
