#include "xre_runtime.h"
#include "xre_utils.h"
#include "xre_compiler.h"
#include <stdbool.h>

static void copy_port(struct pointer *dst, struct pointer *src)
{
	dst->port	= src->port;
	dst->known_port = src->known_port;
}

// static struct pointer *lowerize_type()

static struct pointer *assign_type(
	struct expression_meta *meta,
	struct pointer	       *left_pointer,
	struct pointer	       *right_pointer,
	struct pointer	       *self_pointer
)
{
	if (meta->type_rule == RETURN_TYPE_RULE_LEFT) {
		if (left_pointer &&
		    __pointer_known_port(*left_pointer)) {
			copy_port(self_pointer, left_pointer);
			goto return_pointer;
		}

	} else if (meta->type_rule == RETURN_TYPE_RULE_RIGHT) {
		if (right_pointer &&
		    __pointer_known_offset(*right_pointer)) {
			copy_port(self_pointer, right_pointer);
			goto return_pointer;
		}

	} else if (meta->type_rule == RETURN_TYPE_RULE_INHERIT) {
		if (left_pointer->port.type ==
		    right_pointer->port.type) {
			copy_port(self_pointer, left_pointer);
		}
	}

return_pointer:
	if (meta->offset_rule == RETURN_OFFSET_RULE_YIELD) {
		self_pointer->port.protection =
			meta->profile.ret.protection;
	}

	return (self_pointer);
}

/* DEFINE GLOBAL */
static struct vector *global_scope = NULL;

/* DEFINE GLOBAL */
static size_t global_scope_size = 0;

static void push_locals_to_scope(struct expression *node)
{
	struct vector *locals	= __node_locals(node);
	size_t	       n_locals = vec_size(locals);

	(void)vec_append(global_scope, locals->_ptr, n_locals);
	global_scope_size += n_locals;
}

static struct pointer *analyzer(struct expression *node)
{
	struct pointer *left_pointer  = NULL;
	struct pointer *right_pointer = NULL;

	struct expression_meta *meta = find_expression_meta(node);

	if (vec_size(__node_locals(node))) {
		push_locals_to_scope(node);
	}

	if (__node_attr_kind(node) == REFERENCE) {
		return (&__node_as_reference(node));
	}

	if (__node_token_kind(node) == __SEQUENCE_POINT__) {
		size_t i = 0;
		while (i < vec_size(__node_as_sequence(node))) {
			struct expression *exp = (struct expression *)
				vec_at(__node_as_sequence(node), i++);

			right_pointer = analyzer(exp);
		}

		__node_pointer(node).port.protection = PROT_NONE;
		__node_pointer(node).port.type	     = TYPE_NONE;

		return (&__node_as_reference(node));
	}

	switch (__node_token_type(node)) {
	case EXPR_OP_TYPE_UNIOP:
		left_pointer = analyzer(__node_as_binop_l(node));

		break;

	case EXPR_OP_TYPE_BINOP:
		left_pointer  = analyzer(__node_as_binop_l(node));
		right_pointer = analyzer(__node_as_binop_r(node));

		break;

	default:

		break;
	}

	if (vec_size(__node_locals(node))) {
		size_t i = 0;
		while (i < vec_size(__node_locals(node))) {
			vec_pop(global_scope, NULL);
			i--;
		}

		global_scope_size -= vec_size(__node_locals(node));
	}

	return (assign_type(
		meta,
		left_pointer,
		right_pointer,
		&__node_pointer(node)
	));
}

bool resolve_return_types(struct expression *node)
{
	global_scope	  = vec_create(sizeof(object_t), 64, NULL);
	global_scope_size = 0;

	(void)analyzer(node);
	return (true);
}
