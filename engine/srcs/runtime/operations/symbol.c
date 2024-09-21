#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

/*    Executes when a symbol is called as
 *    a value.
 */
static inline bool _oper_symbol_expand(ast_stmt_t *self, object_t *object)
{
	return (unwrap_symbol_read(self, self->value, &object)&&  __push_r(self, object));
}

/*    Executes when a symbol is called for
 *    assignment.
 */
static inline bool _oper_symbol_addr(ast_stmt_t *self, object_t *object)
{
	object_symbol_init(self->value, object);
	return (__push_r(self, object));
}

XRE_API(oper_symbol_expand)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	bool ret = _oper_symbol_expand(self, &_result);
	return (ret);
}

XRE_API(oper_symbol_addr)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	bool ret = _oper_symbol_addr(self, &_result);
	return (ret);
}
