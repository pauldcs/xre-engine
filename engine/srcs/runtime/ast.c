#include "xre_assert.h"
#include "xre_runtime.h"
#include "xre_operations.h"
#include "xre_builtin.h"
#include <stdio.h>
#include <string.h>

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
	[__DO__] = oper_do,
	[__ELSE__] = oper_else,
	[__AND__] = oper_and,
	[__OR__] = oper_or,
	[__IDENTIFIER__] = oper_symbol,
	[__ANNOTATE__] = oper_annotate,
	[__INJECT__] = oper_inject,
	[__VAL__] = oper_value,
	[__STRING_LITERAL__] = oper_string,
	[__SEQUENCE__] = oper_sequence,
	[__SEPARATOR__] = oper_separator,
};

static ast_stmt_t *alloc_zeroed_stmt_tree(size_t size)
{
	size_t alloc_size = sizeof(ast_stmt_t) * size;
	ast_stmt_t *tree = malloc(alloc_size);
	if (tree) {
		(void)memset(tree, 0x00, alloc_size);
	}

	return (tree);
}

static size_t get_tree_size(xre_ast_t *ast)
{
	size_t size = 1;

	switch (ast->kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __IDENTIFIER__:
		return (size);

	case __NOT__:
	case __BUILTIN_CALL__:
		return (size + get_tree_size(ast->_binop.left));

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
		current->eval = get_builtin_ptr(ast->token._ptr + 1, ast->token._len -1);
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

	case __IDENTIFIER__:
		current->value = symtab_create_entry((char *)ast->string);
		break;

	case __NOT__:
	case __BUILTIN_CALL__:
		current->br.left = stmt_tree_init(stmt, ast->uniop, false);
		current->br.right = -1;
		break;

	default:
		current->br.left =
			stmt_tree_init(stmt, ast->_binop.left, false);
		current->br.right =
			stmt_tree_init(stmt, ast->_binop.right, false);
		break;
	}

	return (initial_index);
}

ast_stmt_t *stmt_tree_create(xre_ast_t *ast)
{
	ast_stmt_t *tree =
		alloc_zeroed_stmt_tree(get_tree_size(ast) * sizeof(ast_stmt_t));

	if (tree) {
		(void)stmt_tree_init(tree, ast, true);
	}

	return (tree);
}

void stmt_tree_destroy(ast_stmt_t *tree)
{
	free(tree);
}
