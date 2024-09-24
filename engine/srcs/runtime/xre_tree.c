#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

static vec_t *_local_cache = NULL;

static bool tree_node_alloc(struct statements **statement)
{
	*statement = malloc(sizeof(struct statements));
	if (!*statement) {
		return (false);
	}
	(void)memset(*statement, 0x00, sizeof(struct statements));
	return (true);
}

void tree_node_dealloc(struct statements **statement)
{
	(void)free((void *)*statement);
	*statement = NULL;
}

static bool local_cache_alloc(void)
{
	_local_cache = vec_create(sizeof(char *), 64, NULL);
	if (!_local_cache) {
		return (false);
	}
	return (true);
}

static bool local_cache_push(const char *str)
{
	return (vec_push(_local_cache, &str));
}

static void local_cache_pop(void)
{
	const char *str;
	vec_pop(_local_cache, &str);
}

static void local_cache_dealloc(void)
{
	(void)vec_kill(_local_cache);
	_local_cache = NULL;
}

static ssize_t is_cached(const char *str)
{
	size_t size = vec_size(_local_cache);
	while (size--) {
		const char *s = *(char **)vec_at(_local_cache, size);
		if (!strcmp(s, str)) {
			return (size);
		}
	}
	return (-1);
}

static bool statement_is_scope_modifier(struct statements *statement)
{
	switch (statement->meta.source->_kind) {
	case __DO__:
	case __ELSE__:
	case __AND__:
	case __OR__:
	case __CLOSURE__:
	case __LOOP__:
		return (true);

	default:
		return (false);
	}
}

static bool _is_scope_change(struct statements *parent)
{
	if (!parent) {
		return (true);
	}

	return (statement_is_scope_modifier(parent));
}

static const char *format_value(xre_ast_t *ast)
{
	switch (ast->kind) {
	case __VARIABLE__:
		return (format_string("%s", ast->string));
	case __VAL__:
		return (format_string("%d", ast->value));
	case __STRING_LITERAL__:
		return (format_string("'%s'", ast->string));
	case __SEQUENCE__:
		return (format_string("A"));
	default:
		return (format_string("_"));
	}
}

vec_t *_frame	 = NULL;
size_t _n_frames = 0;

static bool runtime_tree_create(
	xre_ast_t	   *ast,
	struct statements **statements,
	bool		    is_scope_change
)
{
	const char *format = NULL;
	const char *fmt	   = NULL;
	ssize_t	    pos	   = -1;
	size_t	    npos   = 0;
	size_t	    n;

	if (!tree_node_alloc(statements)) {
		return (false);
	}

	struct statements *node = *statements;

	node->meta.iden	  = expr_kind_to_string(ast->kind);
	node->meta.source = (xre_token_t *)&ast->token;
	node->frame	  = vec_create(sizeof(char *), 4, NULL);
	node->_op	  = NULL;

	if (_frame == NULL) {
		_frame = node->frame;
	}

	vec_t *save_frame = _frame;

	if (is_scope_change) {
		_n_frames += vec_size(_frame);
		_frame = node->frame;
	}

	if (node->meta.source->_kind == __SEQUENCE_POINT__) {
		n		    = vec_size(ast->seq);
		node->self.children = vec_create(
			sizeof(struct statements), n, NULL
		);
		for (size_t i = 0; i < n; i++) {
			struct statements *statement;
			if (!runtime_tree_create(
				    vec_access(ast->seq, i),
				    &statement,
				    is_scope_change
			    )) {
				return (false);
			}

			vec_push(node->self.children, statement);
		}
		goto end;
	}

	switch (ast->type) {
	case EXPR_TYPE_VALUE:
		format = format_value(ast);
		pos    = is_cached(format);
		npos   = 0;

		if (pos == -1) {
			npos = vec_size(save_frame);
		} else {
			npos = pos;
		}

		switch (ast->kind) {
		case __VARIABLE__:
			fmt = format_string(
				"local: '%s'", ast->string
			);
			break;

		case __VAL__:
			fmt = format_string("static: %d", ast->value);
			break;

		case __STRING_LITERAL__:
			fmt = format_string(
				"static string: '%s'", ast->string
			);
			break;

		default:
			fmt = format_string("_");
			break;
		}

		node->meta.iden	  = format;
		node->self.offset = _n_frames + npos;

		if (pos == -1) {
			local_cache_push(format);
			vec_push(_frame, &fmt);
		}

		break;

	case EXPR_OP_TYPE_UNIOP:
		if (!runtime_tree_create(
			    ast->_binop.left,
			    &__binop_unfold_left(node),
			    false
		    )) {
			return (false);
		}

		break;

	case EXPR_OP_TYPE_BINOP:
		if (!runtime_tree_create(
			    ast->_binop.left,
			    &__binop_unfold_left(node),
			    false
		    )) {
			return (false);
		}

		if (!runtime_tree_create(
			    ast->_binop.right,
			    &__binop_unfold_right(node),
			    _is_scope_change(node)
		    )) {
			return (false);
		}
		break;

	default:
		break;
	}

end:
	if (is_scope_change) {
		_n_frames -= vec_size(node->frame);
		size_t n = vec_size(node->frame);
		while (n--) {
			local_cache_pop();
		}
	}

	*statements = node;
	return (true);
}

bool runtime_tree_init(xre_ast_t *ast, struct runtime *runtime)
{
	if (!local_cache_alloc()) {
		return (false);
	}

	bool ret = runtime_tree_create(ast, &runtime->start, true);

	local_cache_dealloc();

	return (ret);
}
