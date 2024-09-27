#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>

static size_t stack_size = 0;

static int64_t create_ret_type(
	struct op_meta *op_meta, int64_t ret_left, int64_t ret_right
)
{
	if (op_meta->_ret & O_BR_RESULT_LEFT) {
		return (ret_left);

	} else if (op_meta->_ret & O_BR_RESULT_RIGHT) {
		return (ret_right);

	}

	return (-1);
}

int64_t eval_return_offsets(struct statements *node)
{
        int64_t ret_left = 0;
        int64_t ret_right = 0;

	struct op_meta *op_meta =
		get_expr_types(__statement_kind(node));

	size_t i = 0;
	while (i < vec_size(__statement_frame(node))) {
		stack_size++;
		i++;
	}
	
	if (__statement_type(node) == EXPR_TYPE_VALUE) {
                return (__statement_offset(node));
	}

	if (__statement_kind(node) == __SEQUENCE_POINT__) {
		while (i < vec_size(__statement_children(node))) {
			while (i <
			       vec_size(__statement_children(node))) {
				ret_left = eval_return_offsets(
					(struct statements *)vec_at(
						__statement_children(
							node
						),
                                                i++
					)
				);
			}
		}
		goto end;
	}

	switch (__statement_type(node)) {
	case EXPR_OP_TYPE_UNIOP:
		ret_left =
			eval_return_offsets(__statement_left(node));

		break;

	case EXPR_OP_TYPE_BINOP:
		ret_left =
			eval_return_offsets(__statement_left(node));

		ret_right =
			eval_return_offsets(__statement_right(node));

		break;

	default:

		break;
	}
end:

	if (vec_size(node->frame)) {
		stack_size -= vec_size(node->frame);
	}

	int64_t ret = create_ret_type(op_meta, ret_left, ret_right);
        node->return_offset = ret;
	return (ret);
}
