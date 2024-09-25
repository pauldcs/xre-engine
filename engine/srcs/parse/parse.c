#include "vec.h"
#include "xre_compiler.h"
#include "xre_builtin.h"
#include "xre_errors.h"
#include "xre_parse.h"
#include "xre_nodes.h"
#include "xre_utils.h"
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

#define MICROSTACK_SIZE 2048
static const void *__stack_a[MICROSTACK_SIZE];
static const void *__stack_b[MICROSTACK_SIZE];
static size_t	   __top_a;
static size_t	   __top_b;

#define TOP_A_KIND ((struct ast *)__stack_a[__top_a])->kind
#define TOP_B_KIND ((struct ast *)__stack_b[__top_b])->kind

static void __push_a(struct ast *node)
{
	if (__top_a < MICROSTACK_SIZE - 1)
		__stack_a[++__top_a] = node;
}
static void __push_b(struct ast *node)
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

static struct ast *ast_new_node(struct token *token)
{
	struct ast *node = malloc(sizeof(struct ast));

	bzero(node, sizeof(struct ast));
	node->kind = token->_kind;
	node->type = token->_type;

	(void
	)memcpy((void *)&node->token, token, sizeof(struct token));

	if (node->kind == __VAL__)
		node->value = token->_value;

	if (node->kind == __STRING_LITERAL__) {
		char *tmp = strndup(token->_ptr + 1, token->_len - 2);
		(void)str_unescape(tmp);
		node->string = tmp;
	}

	if (node->kind == __VARIABLE__) {
		node->string = strndup(token->_ptr, token->_len);
	}

	if (node->kind == __BUILTIN_CALL__) {
		node->string =
			builtin_get_name(token->_ptr, token->_len);
	}

	return (node);
}

static bool unfold_sequence_ast(struct ast *ast, vec_t *buffer)
{
	if (ast->kind == __SEQUENCE__) {
		if (!vec_concat(buffer, ast->seq)) {
			return (false);
		}

		//vec_kill(ast->seq);

	} else {
		if (!vec_append(buffer, ast, 1)) {
			return (false);
		}
	}
	return (true);
}

bool sequence_node(
	struct ast *node,
	size_t	    depth,
	struct ast *lval,
	struct ast *rval
)
{
	__return_val_if_fail__(lval, NULL);
	__return_val_if_fail__(rval, NULL);

	vec_t *sequence = vec_create(sizeof(struct ast), 8, free);

	if (lval->token._depth > rval->token._depth) {
		if (!vec_append(sequence, lval, 1) ||
		    !unfold_sequence_ast(rval, sequence)) {
			goto prison;
		}
	} else {
		if (rval->token._depth != depth) {
			if (!vec_append(sequence, lval, 1) ||
			    !vec_append(sequence, rval, 1)) {
				goto prison;
			}
		} else if (!unfold_sequence_ast(lval, sequence) ||
			   !unfold_sequence_ast(rval, sequence)) {
			goto prison;
		}
	}

	node->seq  = sequence;
	node->kind = __SEQUENCE__;
	node->type = EXPR_OP_TYPE_SEQUENCE;
	return (true);

prison:
	vec_kill(sequence);
	return (false);
}

static void __make_value_to_b(void)
{
	struct ast *node = __pop_a();
	void	   *a;
	void	   *b;
	assert(node);

	if (node->type & EXPR_OP_TYPE_UNIOP) {
		node->uniop = __pop_b();

	} else {
		if (node->kind == __SEQUENCE_POINT__) {
			a = __pop_b();
			b = __pop_b();
			(void)sequence_node(
				node, node->token._depth, a, b
			);
		} else {
			node->_binop.right = __pop_b();
			node->_binop.left  = __pop_b();
		}
	}

	__push_b(node);
}

struct ast *xre_expr_parse(vec_t *tokens)
{
	__return_val_if_fail__(tokens, NULL);

	struct token *token = NULL;
	size_t	      idx   = 0;

	__top_a = 0;
	__top_b = 0;

	(void)memset(__stack_a, 0, MICROSTACK_SIZE * sizeof(void *));
	(void)memset(__stack_b, 0, MICROSTACK_SIZE * sizeof(void *));

	while (true) {
		token = (struct token *)vec_unsafe_at(tokens, ++idx);
		if (!token)
			break;

		if (token->_kind == __END__)
			break;

		switch (token->_kind) {
		case __LBRACK__:
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
		case __RBRACK__:
			while (__top_a && (TOP_A_KIND != __LBRACK__))
				__make_value_to_b();

			free((void *)__stack_a[__top_a]);
			__pop_a();

			break;
		case __VAL__:
		case __STRING_LITERAL__:
		case __VARIABLE__:
			__push_b(ast_new_node(token));

			break;
		default:
			while (__top_a &&
			       (get_precedence_by_kind(TOP_A_KIND) >=
				get_precedence_by_kind(token->_kind))
			) {
				__make_value_to_b();
			}

			__push_a(ast_new_node(token));
		}
	}
	while (__top_a)
		__make_value_to_b();

	while (__top_b > 1)
		__make_value_to_b();

	return ((struct ast *)__stack_b[__top_b]);
}
