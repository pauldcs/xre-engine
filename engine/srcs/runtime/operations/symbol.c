#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

// /*    Executes when a symbol is called as
//  *    a value.
//  */
// static inline bool
// _oper_symbol_expand(struct statement *self, object_t *object)
// {
// 	return (unwrap_symbol_read(self, self->value, &object) &&
// 		__push_r(self, object));
// }

// /*    Executes when a symbol is called for
//  *    assignment.
//  */
// static inline bool
// _oper_symbol_addr(struct statement *self, object_t *object)
// {
// 	object_symbol_init(self->value, object);
// 	return (__push_r(self, object));
// }

static inline bool _oper_symbol(struct statement *self)
{
	return (set_current_error(self, XRE_NOT_IMPLEMENTED_ERROR),
		false);
}

XRE_API(oper_symbol)
{
	__trigger_bug_if(self == NULL);
	return (_oper_symbol(self));
}

// XRE_API(oper_symbol_addr)
// {
// 	__trigger_bug_if(self == NULL);
// 	static object_t _result = { 0 };

// 	bool ret = _oper_symbol_addr(self, &_result);
// 	return (ret);
// }
