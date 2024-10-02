#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>

static int64_t create_return_attrs(
	struct operation_info *operation_info,
	int64_t		       left_attrs,
	int64_t		       right_attrs
)
{
	int64_t attrs = operation_info->_ret &
			(O_ATTR_MASK | O_BR_MASK);

	if (operation_info->_ret & O_BR_RESULT_LEFT) {
		attrs |= (left_attrs & O_TYPE_MASK);

	} else if (operation_info->_ret & O_BR_RESULT_RIGHT) {
		attrs |= (right_attrs & O_TYPE_MASK);

	} else if (operation_info->_ret & O_BR_RESULT_ANY) {
		attrs |= ((left_attrs | right_attrs) & O_TYPE_MASK);

	} else {
		attrs |= (operation_info->_ret & O_TYPE_MASK);
	}

	return (attrs);
}

/* DEFINE GLOBAL */
static size_t stack_size = 0;

int64_t analyzer(__ast_node *node)
{
	int64_t left_attrs  = 0;
	int64_t right_attrs = 0;

	struct operation_info *operation_info =
		operation_info_lookup_kind(__expression_kind(node));

	size_t i = 0;
	while (i < vec_size(__expression_frame_locals(node))) {
		stack_size++;
		i++;
	}

	if (__expression_type(node) == EXPR_TYPE_VALUE) {
		return (operation_info->_ret);
	}

	if (__expression_kind(node) == __SEQUENCE_POINT__) {
		i		      = 0;
		int64_t initial_attrs = O_TYPE_UNDEFINED;

		bool all_read  = true;
		bool all_const = true;

		while (i < vec_size(__expression_sequence(node))) {
			__ast_node *exp = (__ast_node *)vec_at(
				__expression_sequence(node), i++
			);

			right_attrs = analyzer(exp);
			if (!(right_attrs & O_ATTR_READABLE)) {
				all_read = false;
			}

			if (right_attrs & O_ATTR_MUTABLE) {
				all_const = false;
			}
		}
		if (all_read) {
			initial_attrs |= O_ATTR_READABLE;
		}
		if (!all_const) {
			initial_attrs |= O_ATTR_MUTABLE;
		}

		__expression_dest_pmask(node) = initial_attrs;
		return (initial_attrs);
	}

	switch (__expression_type(node)) {
	case EXPR_OP_TYPE_UNIOP:
		left_attrs = analyzer(__expression_binop_left(node));

		break;

	case EXPR_OP_TYPE_BINOP:
		left_attrs = analyzer(__expression_binop_left(node));

		right_attrs =
			analyzer(__expression_binop_right(node));

		break;

	default:

		break;
	}

	if (vec_size(__expression_frame_locals(node))) {
		stack_size -=
			vec_size(__expression_frame_locals(node));
	}

	struct operation_info o;
	if (__expression_kind(node) == __BUILTIN_CALL__) {
		o._ret = __expression_builtin(node)->self_attr;
		o._p1  = __expression_builtin(node)->left_attr;
		o._p2  = __expression_builtin(node)->right_attr;

		int64_t ret = create_return_attrs(
			&o, left_attrs, right_attrs
		);

		__expression_dest_pmask(node) = ret;
		return (ret);
	}

	int64_t attrs = create_return_attrs(
		operation_info, left_attrs, right_attrs
	);
	__expression_dest_pmask(node) = attrs;
	return (attrs);
}

bool resolve_return_types(__ast_node *node)
{
	stack_size = 0;
	(void)analyzer(node);
	return (true);
}
