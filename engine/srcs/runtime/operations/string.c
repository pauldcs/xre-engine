#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>
#include <string.h>

XRE_API_OPERATOR_FUNC(oper_string)
{
	__return_val_if_fail__(self, false);

	return (__push_r(self,
			 object_string_create((unsigned char *)self->string,
					      strlen(self->string))));
}
