#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>

struct vector *scope = NULL;

int64_t is_assign_left = false;
int64_t right_offset   = -1;

void eval_variable_flow(struct expression *node)
{
	if (!scope) {
		scope = vec_create(sizeof(object_t), 64, NULL);
	}

	size_t i = 0;
	while (i < vec_size(__expression_locals(node))) {
		(void)vec_push(
			scope,
			(object_t *)vec_access(
				__expression_locals(node), i
			)
		);
		i++;
	}

	if (__expression_type(node) == EXPR_TYPE_VALUE) {
		object_t *object = vec_access(
			scope, __expression_ref_offset(node)
		);

		if (is_assign_left) {
			if (right_offset != -1) {
				object_t *ob_right = vec_access(
					scope, right_offset
				);
				if ((__object_objattrs(object) &
				     O_TYPE_UNDEFINED)) {
					__object_objattrs(object
					) = __object_objattrs(ob_right
					    ) |
					    O_ATTR_MUTABLE;
					__expression_ref_access_mask(
						node
					) = __object_objattrs(object);
				}
			}
		}

		__expression_ref_access_mask(node) =
			__object_objattrs(object);
		return;
	}

	i = 0;

	if (__expression_kind(node) == __SEQUENCE_POINT__) {
		i = 0;
		while (i < vec_size(__expression_sequence(node))) {
			eval_variable_flow((struct expression *)vec_at(
				__expression_sequence(node), i++
			));
		}
		goto end;
	}

	switch (__expression_type(node)) {
	case EXPR_TYPE_VALUE:
		break;

	case EXPR_OP_TYPE_UNIOP:
		eval_variable_flow(__expression_binop_left(node));
		break;

	case EXPR_OP_TYPE_BINOP:
		if (__expression_kind(node) == __ASSIGN__) {
			is_assign_left = false;
			right_offset   = -1;

			eval_variable_flow(
				__expression_binop_right(node)
			);

			is_assign_left = true;
			right_offset = __expression_dest_offset(node);

			eval_variable_flow(
				__expression_binop_left(node)
			);

			is_assign_left = false;
			right_offset   = -1;

		} else {
			eval_variable_flow(
				__expression_binop_left(node)
			);
			eval_variable_flow(
				__expression_binop_right(node)
			);
		}

		break;

	default:

		break;
	}
end:

	if (vec_size(__expression_locals(node))) {
		size_t i = vec_size(__expression_locals(node));
		while (i--) {
			vec_pop(scope, NULL);
		}
	}
}
