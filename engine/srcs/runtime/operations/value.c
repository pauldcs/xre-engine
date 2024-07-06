#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_value)
{
	__return_val_if_fail__(self, false);

	return (stack_push_enable_attrs(self,
					object_create_register(self->value),
					ATTR_READABLE | ATTR_MUTABLE));
}
