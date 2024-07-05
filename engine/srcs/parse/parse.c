#include "array.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_parse.h"
#include "xre_builtin.h"
#include "xre_utils.h"
#include <assert.h>
#include <string.h>
#include <sys/types.h>

#define MICROSTACK_SIZE 2048
static const void *__stack_a[MICROSTACK_SIZE];
static const void *__stack_b[MICROSTACK_SIZE];
static size_t __top_a;
static size_t __top_b;

#define TOP_A_KIND ((xre_ast_t *)__stack_a[__top_a])->kind
#define TOP_B_KIND ((xre_ast_t *)__stack_b[__top_b])->kind

static void __push_a(xre_ast_t *node)
{
	if (__top_a < MICROSTACK_SIZE - 1)
		__stack_a[++__top_a] = node;
}
static void __push_b(xre_ast_t *node)
{
	if (__top_b < MICROSTACK_SIZE - 1)
		__stack_b[++__top_b] = node;
}
static void *__pop_a(void)
{
	if (__top_a > 0)
		return ((void *)__stack_a[__top_a--]);
	return ((void *)0);
}
static void *__pop_b(void)
{
	if (__top_b > 0)
		return ((void *)__stack_b[__top_b--]);
	return ((void *)0);
}

static xre_ast_t *ast_new_node(xre_token_t *token)
{
	xre_ast_t *node = malloc(sizeof(xre_ast_t));

	bzero(node, sizeof(xre_ast_t));
	node->kind = token->_kind;
	node->type = token->_type;

	(void)memcpy((void *)&node->token, token, sizeof(xre_token_t));

	if (node->kind == __VAL__)
		node->value = token->_value;

	if (node->kind == __STRING_LITERAL__) {
		char *tmp = strndup(token->_ptr + 1, token->_len - 2);
		node->string = tmp;
		str_unescape(tmp);
	}

	if (node->kind == __IDENTIFIER__) {
		node->string = strndup(token->_ptr, token->_len);
	}

	if (node->kind == __BUILTIN_CALL__) {
		node->string = get_builtin_name_ptr(token->_ptr, token->_len);
	}

	return (node);
}

static void __make_value_to_b(void)
{
	xre_ast_t *node = __pop_a();
	assert(node);

	if (node->type & EXPR_OP_TYPE_UNIOP) {
		node->uniop = __pop_b();

	} else {
		node->_binop.right = __pop_b();
		node->_binop.left = __pop_b();
	}

	__push_b(node);
}

xre_ast_t *xre_expr_parse(array_t *tokens)
{
	__return_val_if_fail__(tokens, NULL);

	xre_token_t *token = NULL;
	size_t idx = 0;

	__top_a = 0;
	__top_b = 0;

	memset(__stack_a, 0, MICROSTACK_SIZE * sizeof(void *));
	memset(__stack_b, 0, MICROSTACK_SIZE * sizeof(void *));

	while (true) {
		token = (xre_token_t *)array_unsafe_at(tokens, ++idx);
		if (!token)
			break;

		if (token->_kind == __END__)
			break;

		switch (token->_kind) {
		case __LPAREN__:
			__push_a(ast_new_node(token));

			break;

		case __NOT__:
		case __BUILTIN_CALL__:
			__push_a(ast_new_node(token));

			break;

		case __RPAREN__:
			while (__top_a && (TOP_A_KIND != __LPAREN__))
				__make_value_to_b();

			free((void *)__stack_a[__top_a]);
			__pop_a();

			break;
		case __VAL__:
		case __STRING_LITERAL__:
		case __IDENTIFIER__:
			__push_b(ast_new_node(token));

			break;
		default:
			while (__top_a &&
			       (get_precedence_by_kind(TOP_A_KIND) >=
				get_precedence_by_kind(token->_kind))) {
				__make_value_to_b();
			}

			__push_a(ast_new_node(token));
		}
	}
	while (__top_a)
		__make_value_to_b();

	while (__top_b > 1)
		__make_value_to_b();

	return ((xre_ast_t *)__stack_b[__top_b]);
}
