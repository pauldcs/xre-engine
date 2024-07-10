#ifndef __XRE_OPERATIONS_H__
#define __XRE_OPERATIONS_H__

#include "xre_memory.h"
#include "xre_runtime.h"
#include <stdbool.h>

bool binop_evaluate_pop_r(ast_stmt_t *self, object_t *left_buffer,
			  object_t *right_buffer);

bool stack_pop_r(object_t *ptr, ast_stmt_t *stmts);
bool stack_pop_r_binop(ast_stmt_t *self, object_t *left_buffer,
		       object_t *right_buffer);

bool stack_push_enable_attrs(ast_stmt_t *self, object_t *object, int32_t attrs);
void set_current_error(ast_stmt_t *self, error_type_e type);

bool unwrap_register_object(ast_stmt_t *self, object_t *object, int64_t *data);
bool unwrap_symbol_read(ast_stmt_t *self, int64_t offset, object_t **object);
bool unwrap_symbol_write(ast_stmt_t *self, int64_t offset, object_t **object);

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
XRE_API_OPERATOR_FUNC(oper_symbol_expand);
XRE_API_OPERATOR_FUNC(oper_symbol_addr);
XRE_API_OPERATOR_FUNC(oper_annotate);
XRE_API_OPERATOR_FUNC(oper_separator);
XRE_API_OPERATOR_FUNC(oper_sequence);
XRE_API_OPERATOR_FUNC(oper_scope_resolution);
XRE_API_OPERATOR_FUNC(oper_inject);
XRE_API_OPERATOR_FUNC(oper_scope_annotate);
XRE_API_OPERATOR_FUNC(oper_print);

#define __left_branch (&__global_current_stmts_ptr__[self->br.left])
#define __right_branch (&__global_current_stmts_ptr__[self->br.right])

#endif /* __XRE_OPERATIONS_H__ */
