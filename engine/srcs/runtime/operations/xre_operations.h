#ifndef __XRE_OPERATIONS_H__
#define __XRE_OPERATIONS_H__

#include "xre_runtime.h"
#include "xre_memory.h"
#include "xre_object.h"
#include <stdbool.h>

bool pop_object(object_t *ptr, ast_stmt_t *stmts);
bool evaluate_binops(ast_stmt_t *self, object_t *left_buffer,
		     object_t *right_buffer);
bool pop_binop_return(ast_stmt_t *self, object_t *left_buffer,
		      object_t *right_buffer);
bool stack_push_flagged(ast_stmt_t *self, object_t *object, int32_t flags);
void trigger_error_on(ast_stmt_t *self, error_type_e type);

#define XRE_API_OPERATOR_FUNC(name) bool name(ast_stmt_t *self)
typedef bool (*fptr_t)(ast_stmt_t *self);

XRE_API_OPERATOR_FUNC(oper_add);
XRE_API_OPERATOR_FUNC(oper_bw_and);
XRE_API_OPERATOR_FUNC(oper_bw_or);
XRE_API_OPERATOR_FUNC(oper_bw_xor);
XRE_API_OPERATOR_FUNC(oper_div);
XRE_API_OPERATOR_FUNC(oper_lshift);
XRE_API_OPERATOR_FUNC(oper_rshift);
XRE_API_OPERATOR_FUNC(oper_mod);
XRE_API_OPERATOR_FUNC(oper_mul);
XRE_API_OPERATOR_FUNC(oper_pow);
XRE_API_OPERATOR_FUNC(oper_sub);
XRE_API_OPERATOR_FUNC(oper_assign);
XRE_API_OPERATOR_FUNC(oper_and);
XRE_API_OPERATOR_FUNC(oper_do);
XRE_API_OPERATOR_FUNC(oper_else);
XRE_API_OPERATOR_FUNC(oper_loop);
XRE_API_OPERATOR_FUNC(oper_or);
XRE_API_OPERATOR_FUNC(oper_eq);
XRE_API_OPERATOR_FUNC(oper_ge);
XRE_API_OPERATOR_FUNC(oper_gt);
XRE_API_OPERATOR_FUNC(oper_le);
XRE_API_OPERATOR_FUNC(oper_lt);
XRE_API_OPERATOR_FUNC(oper_ne);
XRE_API_OPERATOR_FUNC(oper_not);
XRE_API_OPERATOR_FUNC(oper_string);
XRE_API_OPERATOR_FUNC(oper_value);
XRE_API_OPERATOR_FUNC(oper_symbol);
XRE_API_OPERATOR_FUNC(oper_symbol_addr);
XRE_API_OPERATOR_FUNC(oper_annotate);
XRE_API_OPERATOR_FUNC(oper_separator);
XRE_API_OPERATOR_FUNC(oper_sequence);
XRE_API_OPERATOR_FUNC(oper_scope_resolution);
XRE_API_OPERATOR_FUNC(oper_inject);
XRE_API_OPERATOR_FUNC(oper_scope_annotate);
XRE_API_OPERATOR_FUNC(oper_print);

#define TYPE_CHECK_NEXT(self, flag)                         \
	((stack_top()->flags & flag) ?                      \
		 true :                                     \
		 (set_error_type(XRE_TYPE_MISSMATCH_ERROR), \
		  set_error_orig(self), false))

#define TYPE_CHECK(object, flag) (object->flags & flag)

#define __left_branch (&__global_current_stmts_ptr__[self->br.left])
#define __right_branch (&__global_current_stmts_ptr__[self->br.right])

#define IS_ERROR_RETURN is_true_object((object_t *)stack_top())

#endif /* __XRE_OPERATIONS_H__ */
