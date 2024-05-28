#ifndef __XRE_OPERATIONS_H__
#define __XRE_OPERATIONS_H__

#include "xre_runtime.h"
#include "xre_memory.h"
#include "xre_object.h"
#include <stdbool.h>

# define XRE_OPERATOR_API(name) bool name(ast_stmt_t *self)
typedef bool (*fptr_t)(ast_stmt_t *self);

XRE_OPERATOR_API(oper_add);
XRE_OPERATOR_API(oper_bw_and);
XRE_OPERATOR_API(oper_bw_or);
XRE_OPERATOR_API(oper_bw_xor);
XRE_OPERATOR_API(oper_div);
XRE_OPERATOR_API(oper_lshift);
XRE_OPERATOR_API(oper_rshift);
XRE_OPERATOR_API(oper_mod);
XRE_OPERATOR_API(oper_mul);
XRE_OPERATOR_API(oper_pow);
XRE_OPERATOR_API(oper_sub);
XRE_OPERATOR_API(oper_assign);
XRE_OPERATOR_API(oper_and);
XRE_OPERATOR_API(oper_do);
XRE_OPERATOR_API(oper_else);
XRE_OPERATOR_API(oper_loop);
XRE_OPERATOR_API(oper_or);
XRE_OPERATOR_API(oper_eq);
XRE_OPERATOR_API(oper_ge);
XRE_OPERATOR_API(oper_gt);
XRE_OPERATOR_API(oper_le);
XRE_OPERATOR_API(oper_lt);
XRE_OPERATOR_API(oper_ne);
XRE_OPERATOR_API(oper_not);
XRE_OPERATOR_API(oper_string);
XRE_OPERATOR_API(oper_value);
XRE_OPERATOR_API(oper_symbol);
XRE_OPERATOR_API(oper_symbol_addr);
XRE_OPERATOR_API(oper_annotate);
XRE_OPERATOR_API(oper_separator);
XRE_OPERATOR_API(oper_sequence);
XRE_OPERATOR_API(oper_scope_resolution);
XRE_OPERATOR_API(oper_inject);
XRE_OPERATOR_API(oper_scope_annotate);
XRE_OPERATOR_API(oper_print);

#define TYPE_CHECK_NEXT(self, flag) \
    ((stack_top()->flags & flag)                \
				? true                                      \
				: (                                         \
					set_error_type(XRE_TYPE_MISSMATCH_ERROR), \
					set_error_orig(self),                     \
					false                                     \
				))     

#define TYPE_CHECK(object, flag) (object->flags & flag)

#define LEFT_BRANCH (&__statements__[self->br.left])
#define RIGHT_BRANCH (&__statements__[self->br.right])

#define ECHO_DOLLAR_QUESTION is_true_object((object_t *)stack_top())

#endif /* __XRE_OPERATIONS_H__ */
