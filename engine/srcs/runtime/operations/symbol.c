#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_symbol_expand)
{
	__return_val_if_fail__(self, false);

	static object_t *object;

	return (expand_symbol_read(self, self->value, &object) &&
		stack_push_enable_attrs(self, object, ATTR_READABLE));
}

XRE_API_OPERATOR_FUNC(oper_symbol_addr)
{
	__return_val_if_fail__(self, false);

	return (stack_push_enable_attrs(self, object_create_symbol(self->value),
					ATTR_READABLE));
}
