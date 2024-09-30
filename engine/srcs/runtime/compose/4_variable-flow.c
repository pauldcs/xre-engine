#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>

struct vector *scope = NULL;

int64_t assign_next_ref = false;
int64_t assign_src_offset   = -1;

void eval_variable_flow(struct expression *node)
{
	if (!scope) {
		scope = vec_create(sizeof(object_t *), 64, NULL);
	}

	size_t i = 0;
	while (i < vec_size(__expression_frame_locals(node))) {
		object_t *op = (object_t *)vec_access(
			__expression_frame_locals(node), i
		);

		(void)vec_push(scope, &op);
		i++;
	}

	if (__expression_type(node) == EXPR_TYPE_VALUE) {
		object_t *this_object = *(object_t **)vec_access(
			scope, __expression_ref_offset(node)
		);

		if (assign_next_ref && assign_src_offset > -1) {
			object_t *src =
				*(object_t **)vec_access(
					scope, assign_src_offset
				);
			
			int64_t *dst_attr = &__object_objattrs(this_object);
			int64_t *src_attr = &__object_objattrs(src);

			if (*dst_attr & O_TYPE_UNDEFINED) {
				*dst_attr = (*src_attr | O_ATTR_MUTABLE);
			}
		}

		__expression_ref_pmask(node) = __object_objattrs(this_object);
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
		assign_next_ref = false;

		eval_variable_flow(
			__expression_binop_left(node)
		);
		
		if (__expression_kind(node) == __ASSIGN__) {
			assign_next_ref = true;
			assign_src_offset = __expression_dest_offset(node);
		
		} else {
			assign_next_ref = false;
			assign_src_offset = -1;
		}
		
		eval_variable_flow(
			__expression_binop_right(node)
		);
		
		assign_next_ref = false;
		assign_src_offset = -1;

		break;

	default:

		break;
	}
end:

	if (vec_size(__expression_frame_locals(node))) {
		size_t i = vec_size(__expression_frame_locals(node));
		while (i--) {
			vec_pop(scope, NULL);
		}
	}
}
