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

static int assign_register(void)
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
	__registers[reg] = 0;
}

static size_t alloc_register(
	struct operation_info *operation_info,
	int64_t		       left_offset,
	int64_t		       right_offset
)
{
	if (operation_info->_ret & O_BR_RESULT_LEFT) {
		if (left_offset == -1) {
			return (-assign_register());
		}
		return (left_offset);
	}
	if (operation_info->_ret & O_BR_RESULT_RIGHT) {
		if (right_offset == -1) {
			return (-assign_register());
		}
		return (right_offset);
	}

	if (operation_info->_ret & O_BR_RESULT_ANY) {
		return (-1);
	}

	if (left_offset != -1 && left_offset < 0) {
		return (left_offset);
	}

	if (right_offset != -1 && right_offset < 0) {
		return (right_offset);
	}

	return (-assign_register());
}

static int64_t analyzer(__ast_node *node)
{
	int64_t left_offset  = -1;
	int64_t right_offset = -1;

	size_t i = 0;
	while (i < vec_size(__expression_frame_locals(node))) {
		stack_size++;
		i++;
	}

	if (__expression_type(node) == EXPR_TYPE_VALUE) {
		is_leaf = true;
		return (__expression_ref_offset(node));
	}

	if (__expression_kind(node) == __SEQUENCE_POINT__) {
		i = 0;
		while (i < vec_size(__expression_sequence(node))) {
			analyzer((__ast_node *)vec_at(
				__expression_sequence(node), i++
			));
		}
		goto end;
	}

	switch (__expression_type(node)) {
	case EXPR_OP_TYPE_UNIOP:
		is_leaf	    = false;
		left_offset = analyzer(__expression_binop_left(node));

		break;

	case EXPR_OP_TYPE_BINOP:
		is_leaf	    = false;
		left_offset = analyzer(__expression_binop_left(node));

		right_offset =
			analyzer(__expression_binop_right(node));

		break;

	default:

		break;
	}
end:

	if (vec_size(__expression_frame_locals(node))) {
		stack_size -=
			vec_size(__expression_frame_locals(node));
	}

	struct operation_info *operation_info =
		operation_info_lookup_kind(__expression_kind(node));

	int64_t offset = alloc_register(
		operation_info, left_offset, right_offset
	);

	if (left_offset > -1 && offset != left_offset) {
		release_register(-left_offset);
	}

	if (right_offset > -1 && offset != right_offset) {
		release_register(-right_offset);
	}
	__expression_dest_offset(node) = offset;

	return (offset);
}

bool resolve_return_locations(__ast_node *node)
{
	stack_size = 0;
	is_leaf = false;

	(void)memset(__registers, 0x00, sizeof(__registers));
	
	__registers[0] = 1;
	__registers[1] = 1;
	
	(void)analyzer(node);
	return (true);
}
