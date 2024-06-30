#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_value)
{
	__return_val_if_fail__(self, false);

	return (stack_push_flagged(self, object_create_register(self->value),
				   FLAG_READABLE | FLAG_MUTABLE));
}
