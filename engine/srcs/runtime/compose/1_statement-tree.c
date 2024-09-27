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
		if (!strcmp(str, key)) {
			return (size);
		}
	}
	return (-1);
}

// static struct cache *object_cache_get(const char *key)
// {
// 	size_t size = vec_size(_local_cache);
// 	while (size--) {
// 		struct cache *entry = (struct cache *)vec_at(_local_cache, size);
// 		if (!strcmp(entry->key, key)) {
// 			return (entry);
// 		}
// 	}
// 	return (NULL);
// }

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

static const char *format_value(struct ast *ast)
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

vec_t *_frame	 = NULL;
size_t _n_frames = 0;

static void frame_push(object_t *object)
{
	vec_push(_frame, object);
}

int __frame_changed = -1;
void frame_propagate_diff(struct statements *n, size_t guard, int diff)
{
	enum expr_type type = n->meta.source->_type;

	if (__frame_changed == -1) {
		__frame_changed = 0;
	}

	if (vec_size(n->frame)) {
		__frame_changed++;
	}

	if (n->meta.source->_kind == __SEQUENCE_POINT__) {
		size_t size = vec_size(n->self.children);

		for (size_t i = 0; i < size; i++) {
			frame_propagate_diff(
				vec_access(n->self.children, i),
				guard,
				diff
			);
		}
		return;
	}

	switch (type) {
	case EXPR_TYPE_VALUE:

		if (__frame_changed &&
		    n->self.offset >= (int64_t)guard) {
			n->self.offset += diff;
		}
		break;

	case EXPR_OP_TYPE_UNIOP:
		frame_propagate_diff(__statement_left(n), guard, diff);
		break;

	case EXPR_OP_TYPE_BINOP:
		frame_propagate_diff(__statement_left(n), guard, diff);
		frame_propagate_diff(
			__statement_right(n), guard, diff
		);
		break;

	default:
		break;
	}
}

static bool runtime_tree_create(
	struct ast	   *ast,
	struct statements **statements,
	bool		    is_scope_change
)
{
	const char *format = NULL;
	ssize_t	    pos	   = -1;
	size_t	    npos   = 0;
	size_t	    n;

	if (!tree_node_alloc(statements)) {
		return (false);
	}

	struct statements *node = *statements;

	node->meta.iden	  = expr_kind_to_string(ast->kind);
	node->meta.source = (struct token *)&ast->token;
	node->frame	  = vec_create(sizeof(object_t), 64, NULL);
	node->_op	  = NULL;

	if (_frame == NULL) {
		_frame = node->frame;
	}

	vec_t *save_frame  = _frame;
	size_t save_nframe = _n_frames;

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

	size_t frame_size_guard = 0;
	switch (ast->type) {
	case EXPR_TYPE_VALUE:
		format = format_value(ast);
		pos    = is_cached(format);
		npos   = 0;

		if (pos == -1) {
			npos = _n_frames + vec_size(save_frame);
		} else {
			npos = pos;
		}

		//printf("%zu %s %zu %zu\n", npos, format, _n_frames, vec_size(save_frame));
		node->meta.iden	  = format;
		node->self.offset = npos;

		if (pos == -1) {
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
			    &__statement_left(node),
			    false
		    )) {
			return (false);
		}

		break;

	case EXPR_OP_TYPE_BINOP:

		if (!runtime_tree_create(
			    ast->_binop.left,
			    &__statement_left(node),
			    false
		    )) {
			return (false);
		}

		frame_size_guard = vec_size(_frame);

		if (!runtime_tree_create(
			    ast->_binop.right,
			    &__statement_right(node),
			    _is_scope_change(node)
		    )) {
			return (false);
		}

		int diff = vec_size(_frame) - frame_size_guard;
		if (diff) {
			frame_propagate_diff(
				__statement_left(node),
				frame_size_guard,
				diff
			);
			__frame_changed = -1;
		}

		break;

	default:
		break;
	}

end:
	if (is_scope_change) {
		_frame	  = save_frame;
		_n_frames = save_nframe;
		size_t n  = vec_size(node->frame);
		while (n--) {
			local_cache_pop();
		}
	}

	*statements = node;
	return (true);
}

bool runtime_tree_init(struct ast *ast, struct runtime *runtime)
{
	bool ret = false;

	if (local_cache_alloc()) {
		ret = runtime_tree_create(ast, &runtime->start, true);
		local_cache_dealloc();
	}
	return (ret);
}
