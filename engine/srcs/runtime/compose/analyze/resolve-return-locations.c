#include "xre_compiler.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>
#include <limits.h>

/* DEFINE GLOBAL */
static size_t stack_size = 0;

/* DEFINE GLOBAL */
bool is_leaf = false;

/* DEFINE GLOBAL */
int __registers[64] = {
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static int assign_register_internal(void)
{
	size_t i = 2;
	while (i < 64 - 2) {
		if (__registers[i] == 0) {
			__registers[i] = 1;
			return (i);
		}
		i++;
	}
	return (-1);
}

static void release_register(int reg)
{
	//printf("%d\n", reg);

	__registers[reg] = 0;
}

static void copy_pointer(struct pointer *dst, struct pointer *src)
{
	(void)memcpy(dst, src, sizeof(struct pointer));
}

static void assign_register(
	struct expression_meta *meta,
	struct pointer	       *left_pointer,
	struct pointer	       *right_pointer,
	struct pointer	       *self_pointer
)
{
	if (meta->offset_rule == RETURN_OFFSET_RULE_LEFT) {
		if (left_pointer &&
		    __pointer_known_offset(*left_pointer)) {
			copy_pointer(self_pointer, left_pointer);
			return;
		}

	} else if (meta->offset_rule == RETURN_OFFSET_RULE_RIGHT) {
		if (right_pointer &&
		    __pointer_known_offset(*right_pointer)) {
			copy_pointer(self_pointer, right_pointer);
			return;
		}

	} else if (meta->offset_rule == RETURN_OFFSET_RULE_INHERIT) {
		// checking
		return;
	}

	__pointer_known_offset(*self_pointer) = true;
	__pointer_offset(*self_pointer) = -assign_register_internal();
}

static struct pointer *analyzer(struct expression *node)
{
	struct pointer *left_pointer  = NULL;
	struct pointer *right_pointer = NULL;

	size_t i = 0;
	while (i < vec_size(__node_locals(node))) {
		stack_size++;
		i++;
	}

	if (__node_token_type(node) == EXPR_TYPE_VALUE) {
		is_leaf = true;
		return (&__node_as_reference(node));
	}

	if (__node_token_kind(node) == __SEQUENCE_POINT__) {
		i = 0;
		while (i < vec_size(__node_as_sequence(node))) {
			analyzer((struct expression *)vec_at(
				__node_as_sequence(node), i++
			));
		}
		goto end;
	}

	switch (__node_token_type(node)) {
	case EXPR_OP_TYPE_UNIOP:
		is_leaf	     = false;
		left_pointer = analyzer(__node_as_binop_l(node));

		break;

	case EXPR_OP_TYPE_BINOP:
		is_leaf	      = false;
		left_pointer  = analyzer(__node_as_binop_l(node));
		right_pointer = analyzer(__node_as_binop_r(node));

		break;

	default:

		break;
	}
end:

	if (vec_size(__node_locals(node))) {
		stack_size -= vec_size(__node_locals(node));
	}

	struct expression_meta *meta = find_expression_meta(node);

	assign_register(
		meta,
		left_pointer,
		right_pointer,
		&__node_pointer(node)
	);

	if (left_pointer && __pointer_known_offset(*left_pointer) &&
	    __pointer_known_offset(__node_pointer(node)) &&
	    __pointer_offset(__node_pointer(node)) !=
		    __pointer_offset(*left_pointer) &&
	    __pointer_offset(*left_pointer) < 0) {
		release_register(-__pointer_offset(*left_pointer));
	}

	if (right_pointer && __pointer_known_offset(*right_pointer) &&
	    __pointer_known_offset(__node_pointer(node)) &&
	    __pointer_offset(__node_pointer(node)) !=
		    __pointer_offset(*right_pointer) &&
	    __pointer_offset(*right_pointer) < 0) {
		release_register(-__pointer_offset(*right_pointer));
	}

	return (&__node_pointer(node));
}

bool resolve_return_locations(struct expression *node)
{
	stack_size = 0;
	is_leaf	   = false;

	(void)memset(__registers, 0x00, sizeof(__registers));

	__registers[0] = 1;
	__registers[1] = 1;

	(void)analyzer(node);
	return (true);
}
