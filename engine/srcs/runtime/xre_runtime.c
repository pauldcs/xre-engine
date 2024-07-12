#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

err_notif_t _error;
ast_stmt_t *__global_current_stmts_ptr__ = NULL;

static runtime_op_t oper_lookup_table[] = {
	[__ASSIGN__] = oper_assign,
	[__ADD__] = oper_add,
	[__SUB__] = oper_sub,
	[__MUL__] = oper_mul,
	[__DIV__] = oper_div,
	[__MOD__] = oper_mod,
	[__POW__] = oper_pow,
	[__BAND__] = oper_bw_and,
	[__BOR__] = oper_bw_or,
	[__BXOR__] = oper_bw_xor,
	[__LSHIFT__] = oper_lshift,
	[__RSHIFT__] = oper_rshift,
	[__EQ__] = oper_eq,
	[__NE__] = oper_ne,
	[__LT__] = oper_lt,
	[__GT__] = oper_gt,
	[__LE__] = oper_le,
	[__GE__] = oper_ge,
	[__NOT__] = oper_not,
	[__LOOP__] = oper_loop,
	[__SCOPE_RESOLUTION__] = oper_scope_resolution,
	[__DO__] = oper_do,
	[__ELSE__] = oper_else,
	[__AND__] = oper_and,
	[__OR__] = oper_or,
	[__VARIABLE__] = oper_symbol_expand,
	[__VAL__] = oper_value,
	[__STRING_LITERAL__] = oper_string,
	[__SEQUENCE__] = oper_sequence,
	[__SEPARATOR__] = oper_separator,
};

static bool alloc_zeroed_stmt_tree(size_t size, ast_stmt_t **buffer)
{
	size_t alloc_size = sizeof(ast_stmt_t) * size;
	*buffer = malloc(alloc_size);
	if (!*buffer) {
		return (false);
	}
	(void)memset(*buffer, 0x00, alloc_size);
	return (true);
}

static size_t get_tree_size(xre_ast_t *ast)
{
	size_t size = 1;

	switch (ast->kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __VARIABLE__:
		return (size);

	case __NOT__:
	case __BUILTIN_CALL__:
		if (ast->type == EXPR_TYPE_VALUE) {
			return (size);
		}

		if (ast->type == EXPR_OP_TYPE_UNIOP) {
			return (size + get_tree_size(ast->_binop.left));
		}

		if (ast->type == EXPR_OP_TYPE_BINOP) {
			// fallthrough
		}

	default:
		return (size + get_tree_size(ast->_binop.left) +
			get_tree_size(ast->_binop.right));
	}
}

static int stmt_tree_init(ast_stmt_t *stmt, xre_ast_t *ast, bool reset_index)
{
	static int index = 0;
	if (reset_index) {
		index = 0;
	}

	ast_stmt_t *current = stmt + index;
	int initial_index = index++;

	current->orig = (xre_token_t *)&ast->token;
	if (ast->kind == __BUILTIN_CALL__) {
		current->eval =
			get_builtin_ptr(ast->token._ptr, ast->token._len);
	} else {
		current->eval = oper_lookup_table[ast->kind];
	}

	switch (ast->kind) {
	case __VAL__:
		current->value = ast->value;
		break;

	case __STRING_LITERAL__:
		current->string = (char *)ast->string;
		break;

	case __VARIABLE__:
		current->value = symtab_entry_create((char *)ast->string);
		break;

	case __NOT__:
	case __BUILTIN_CALL__:
		if (ast->type == EXPR_TYPE_VALUE) {
			break;
		}

		if (ast->type == EXPR_OP_TYPE_UNIOP) {
			current->br.left =
				stmt_tree_init(stmt, ast->uniop, false);
			current->br.right = -1;
			break;
		}

		if (ast->type == EXPR_OP_TYPE_BINOP) {
			// fallthrough
		}

	default:
		current->br.left =
			stmt_tree_init(stmt, ast->_binop.left, false);
		current->br.right =
			stmt_tree_init(stmt, ast->_binop.right, false);
		break;
	}

	return (initial_index);
}

static bool stmt_tree_create(xre_ast_t *ast, ast_stmt_t **buffer)
{
	if (!alloc_zeroed_stmt_tree(get_tree_size(ast) * sizeof(ast_stmt_t),
				    buffer)) {
		return (false);
	}

	(void)stmt_tree_init(*buffer, ast, true);
	return (true);
}

void stmt_tree_destroy(ast_stmt_t *tree)
{
	free(tree);
}

bool xre_runtime(xre_ast_t *ast)
{
	__return_val_if_fail__(ast, false);

	ast_stmt_t *self = NULL;
	object_t result = { 0 };

	if (!symtab_init() || !stmt_tree_create(ast, &self)) {
		goto out_of_memory;
	}

	if (!stack_init()) {
		stmt_tree_destroy(self);
		goto out_of_memory;
	}

	// if (!heap_init()) {
	// 	stmt_tree_destroy(self);
	// 	goto out_of_memory;
	// }

	__global_current_stmts_ptr__ = self;

	if (!self->eval(self)) {
		return (xre_error(&_error), stmt_tree_destroy(self), false);
	}

	return (stack_pop(&result), object_drop(&result),
		stmt_tree_destroy(self), true);

out_of_memory:
	(void)fprintf(stderr, "Out of memory\n");
	return (false);
}
