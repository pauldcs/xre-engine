#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

/*    Executes when a symbol is called as
 *    a value.
 */
XRE_API_OPERATOR_FUNC(oper_symbol_expand)
{
	__return_val_if_fail__(self, false);

	static object_t *object;

	return (unwrap_symbol_read(self, self->value, &object) &&
		__push_r(self, object));
}

/*    Executes when a symbol is called for
 *    assignment.
 */
XRE_API_OPERATOR_FUNC(oper_symbol_addr)
{
	__return_val_if_fail__(self, false);

	return (__push_r(self, object_symbol_create(self->value)));
}
