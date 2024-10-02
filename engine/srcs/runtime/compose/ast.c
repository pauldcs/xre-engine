#include "xre_runtime.h"
#include "xre_compiler.h"
#include "xre_parse.h"
#include "xre_builtin.h"
#include "xre_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

/* DEFINE GLOBAL */
static struct vector *_local_cache = NULL;

static bool tree_node_alloc(__ast_node **expression)
{
	*expression = malloc(sizeof(__ast_node));
	if (!*expression) {
		return (false);
	}
	(void)memset(*expression, 0x00, sizeof(__ast_node));
	return (true);
}

void tree_node_dealloc(__ast_node **expression)
{
	(void)free((void *)*expression);
	*expression = NULL;
}

static bool local_cache_alloc(void)
{
	_local_cache = vec_create(sizeof(const char *), 64, NULL);
	if (!_local_cache) {
		return (false);
	}
	return (true);
}

static bool local_cache_push(const char *key)
{
	return (vec_push(_local_cache, &key));
}

static void local_cache_pop(void)
{
	vec_pop(_local_cache, NULL);
}

static void local_cache_dealloc(void)
{
	(void)vec_kill(_local_cache);
	_local_cache = NULL;
}

static ssize_t is_cached(const char *key)
{
	size_t size = vec_size(_local_cache);

	while (size--) {
		const char *str =
			*(char **)vec_at(_local_cache, size);
		if (strcmp(str, key) == 0) {
			return (size);
		}
	}
	return (-1);
}

static bool expression_is_scope_modifier(const __ast_node *expression)
{
	switch (__expression_kind(expression)) {
	case __DO__:
	case __ELSE__:
	case __AND__:
	case __OR__:
	case __LOOP__:
	case __CLOSURE__:
		return (true);

	default:
		return (false);
	}
}

static bool _is_scope_change(const __ast_node *parent)
{
	if (!parent) {
		return (true);
	}

	return (expression_is_scope_modifier(parent));
}

static const char *format_value(const struct ast *ast)
{
	switch (ast->kind) {
	case __VARIABLE__:
		return (format_string("%s", ast->string));

	case __VAL__:
		return (format_string("%d", ast->value));

	case __STRING_LITERAL__:
		return (format_string("'%s'", ast->string));

	default:
		return (format_string("_"));
	}
}

/* DEFINE GLOBAL */
struct vector *global_scope_vector = NULL;

/* DEFINE GLOBAL */
size_t global_scope_vector_index = 0;

static void frame_push(const object_t *object)
{
	vec_push(global_scope_vector, object);
}

/* DEFINE GLOBAL */
int64_t global_frame_index = -1;
void	frame_propagate_diff(
	   __ast_node *expression, int64_t frame_index_guard, int diff
   )
{
	if (global_frame_index == -1) {
		global_frame_index = 0;
	}

	if (vec_size(__expression_frame_locals(expression))) {
		global_frame_index++;
	}

	if (__expression_kind(expression) == __SEQUENCE_POINT__) {
		size_t sequence_size =
			vec_size(__expression_sequence(expression));

		for (size_t i = 0; i < sequence_size; i++) {
			frame_propagate_diff(
				vec_access(
					__expression_sequence(
						expression
					),
					i
				),
				frame_index_guard,
				diff
			);
		}
		return;
	}

	switch (__expression_type(expression)) {
	case EXPR_TYPE_VALUE:
		if (global_frame_index &&
		    __expression_ref_offset(expression) >=
			    frame_index_guard) {
			__expression_ref_offset(expression) += diff;
		}
		break;

	case EXPR_OP_TYPE_UNIOP:
		frame_propagate_diff(
			__expression_binop_left(expression),
			frame_index_guard,
			diff
		);
		break;

	case EXPR_OP_TYPE_BINOP:
		frame_propagate_diff(
			__expression_binop_left(expression),
			frame_index_guard,
			diff
		);
		frame_propagate_diff(
			__expression_binop_right(expression),
			frame_index_guard,
			diff
		);
		break;

	default:
		break;
	}
}

static bool runtime_tree_create(
	struct ast *ast, __ast_node **statements, bool is_scope_change
)
{
	const char *format	       = NULL;
	ssize_t	    self_cached_offset = -1;
	size_t	    self_offset	       = 0;
	size_t	    n;
	struct port port = { .offset = -1,
			     .pmask  = O_TYPE_UNDEFINED };

	if (!tree_node_alloc(statements)) {
		return (false);
	}

	__ast_node *expression = *statements;

	__expression_origin(expression) = (struct token *)&ast->token;
	__expression_frame_locals(expression) =
		vec_create(sizeof(object_t), 64, NULL);
	__expression_dest(expression) = port;

	if (global_scope_vector == NULL) {
		global_scope_vector =
			__expression_frame_locals(expression);
	}

	struct vector *scope_vector_save = global_scope_vector;
	size_t scope_vector_index_save	 = global_scope_vector_index;

	if (is_scope_change) {
		global_scope_vector_index +=
			vec_size(global_scope_vector);
		global_scope_vector =
			__expression_frame_locals(expression);
	}

	if (__expression_kind(expression) == __BUILTIN_CALL__) {
		__expression_builtin(expression) = builtin_find(
			__expression_origin(expression)->_ptr,
			__expression_origin(expression)->_len
		);
	}

	else if (__expression_kind(expression) ==
		 __SEQUENCE_POINT__) {
		n = vec_size(ast->seq);
		__expression_sequence(expression) =
			vec_create(sizeof(__ast_node), n, NULL);
		for (size_t i = 0; i < n; i++) {
			__ast_node *exp;
			if (!runtime_tree_create(
				    vec_access(ast->seq, i),
				    &exp,
				    is_scope_change
			    )) {
				return (false);
			}
			vec_push(
				__expression_sequence(expression), exp
			);
		}
		goto end;
	}

	size_t frame_size_guard = 0;
	switch (ast->type) {
	case EXPR_TYPE_VALUE:
		format		   = format_value(ast);
		self_cached_offset = is_cached(format);
		self_offset	   = -1;

		if (self_cached_offset == -1) {
			self_offset = global_scope_vector_index +
				      vec_size(scope_vector_save);
		} else {
			self_offset = self_cached_offset;
		}

		__expression_ref_offset(expression) = self_offset;
		__expression_ref_pmask(expression) = O_TYPE_UNDEFINED;

		if (self_cached_offset == -1) {
			object_t *object = NULL;

			switch (ast->kind) {
			case __VARIABLE__:
				(void)object_init(
					&object,
					O_TYPE_UNDEFINED |
						O_ATTR_MUTABLE,
					(uint64_t)NULL
				);
				break;

			case __VAL__:
				(void)object_init(
					&object,
					O_ATTR_READABLE |
						O_TYPE_NUMBER,
					(uint64_t)ast->value
				);
				break;

			case __STRING_LITERAL__:
				(void)object_init(
					&object,
					O_ATTR_READABLE |
						O_TYPE_STRING,
					(uint64_t)ast->string
				);
				break;

			case __SEQUENCE__:
				(void)object_init(
					&object,
					O_ATTR_READABLE |
						O_TYPE_SEQUENCE,
					(uint64_t)ast->seq
				);

			default:
				(void)object_init(
					&object,
					O_TYPE_UNDEFINED |
						O_ATTR_MUTABLE,
					0
				);
				break;
			}

			object->str = format;
			local_cache_push(format);
			frame_push(object);
		} else {
		}

		break;

	case EXPR_OP_TYPE_UNIOP:
		if (!runtime_tree_create(
			    ast->_binop.left,
			    &__expression_binop_left(expression),
			    false
		    )) {
			return (false);
		}

		break;

	case EXPR_OP_TYPE_BINOP:

		if (!runtime_tree_create(
			    ast->_binop.left,
			    &__expression_binop_left(expression),
			    false
		    )) {
			return (false);
		}

		frame_size_guard = vec_size(global_scope_vector);

		if (!runtime_tree_create(
			    ast->_binop.right,
			    &__expression_binop_right(expression),
			    _is_scope_change(expression)
		    )) {
			return (false);
		}

		int diff = vec_size(global_scope_vector) -
			   frame_size_guard;
		if (diff) {
			frame_propagate_diff(
				__expression_binop_left(expression),
				frame_size_guard,
				diff
			);
			global_frame_index = -1;
		}

		break;

	default:
		break;
	}

end:
	if (is_scope_change) {
		global_scope_vector	  = scope_vector_save;
		global_scope_vector_index = scope_vector_index_save;
		size_t n =
			vec_size(__expression_frame_locals(expression)
			);
		while (n--) {
			local_cache_pop();
		}
	}

	*statements = expression;
	return (true);
}

bool runtime_tree_init(struct ast *ast, struct runtime *runtime)
{
	__trigger_bug_if(ast == NULL);
	__trigger_bug_if(runtime == NULL);

	bool ret = false;

	global_scope_vector_index = 0;
	global_scope_vector = NULL;

	if (local_cache_alloc()) {
		ret = runtime_tree_create(ast, &runtime->start, true);
		local_cache_dealloc();
	}
	return (ret);
}
