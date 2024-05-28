#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_OPERATOR_API(oper_symbol)
{
	__return_val_if_fail__(self, false);

	if (!stack_push(symtab_get_entry(self->value))) {
		return (set_error_type(XRE_STACK_OVERFLOW_ERROR),
			set_error_orig(self), false);
	}

	return (true);
}

XRE_OPERATOR_API(oper_symbol_addr)
{
	__return_val_if_fail__(self, false);

	if (!stack_push(object_create_symbol(self->value))) {
		return (set_error_type(XRE_STACK_OVERFLOW_ERROR),
			set_error_orig(self), false);
	}

	return (true);
}
