#include "xre_compiler.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>

static size_t stack_size = 0;

static int64_t create_returned_offset(
	struct operation_info *operation_info,
	int64_t		       left_offset,
	int64_t		       right_offset
)
{
	if (operation_info->_ret & O_BR_RESULT_LEFT) {
		return (left_offset);

	} else if (operation_info->_ret & O_BR_RESULT_RIGHT) {
		return (right_offset);
	}

	return (-1);
}

int64_t eval_return_offsets(struct expression *node)
{
	int64_t left_offset  = -1;
	int64_t right_offset = -1;

	struct operation_info *operation_info =
		operation_info_lookup_kind(__expression_kind(node));

	size_t i = 0;
	while (i < vec_size(__expression_locals(node))) {
		stack_size++;
		i++;
	}

	__expression_dest_offset(node) = -1;

	if (__expression_type(node) == EXPR_TYPE_VALUE) {
		return (__expression_ref_offset(node));
	}

	if (__expression_kind(node) == __SEQUENCE_POINT__) {
		i = 0;
		while (i < vec_size(__expression_sequence(node))) {
			left_offset = eval_return_offsets(
				(struct expression *)vec_at(
					__expression_sequence(node),
					i++
				)
			);
		}
		goto end;
	}

	switch (__expression_type(node)) {
	case EXPR_OP_TYPE_UNIOP:
		left_offset = eval_return_offsets(
			__expression_binop_left(node)
		);

		break;

	case EXPR_OP_TYPE_BINOP:
		left_offset = eval_return_offsets(
			__expression_binop_left(node)
		);

		right_offset = eval_return_offsets(
			__expression_binop_right(node)
		);

		break;

	default:

		break;
	}
end:

	if (vec_size(__expression_locals(node))) {
		stack_size -= vec_size(__expression_locals(node));
	}

	int64_t offset = create_returned_offset(
		operation_info, left_offset, right_offset
	);
	__expression_dest_offset(node) = offset;

	return (offset);
}
