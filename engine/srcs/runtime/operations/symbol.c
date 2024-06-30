#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_symbol)
{
	__return_val_if_fail__(self, false);

	if (!stack_push_flagged(self, symtab_get_entry(self->value),
				FLAG_READABLE)) {
		return (false);
	}

	return (true);
}

XRE_API_OPERATOR_FUNC(oper_symbol_addr)
{
	__return_val_if_fail__(self, false);

	return (stack_push_flagged(self, object_create_symbol(self->value),
				   FLAG_READABLE));
}
