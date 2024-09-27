#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>

vec_t *scope = NULL;
struct scope_entry {
	object_t *object;
};

int64_t is_assign_left = false;
int64_t right_offset = -1;

void eval_variable_flow(struct statements *node)
{
	if (!scope) {
		scope = vec_create(
			sizeof(struct scope_entry), 64, NULL
		);
	}

	size_t i = 0;
	while (i < vec_size(__statement_frame(node))) {
		struct scope_entry entry;

		entry.object = (object_t *)vec_access(
			__statement_frame(node), i
		),

		(void)vec_push(scope, &entry);
		i++;
	}
	
	if (__statement_type(node) == EXPR_TYPE_VALUE) {
		struct scope_entry *entry =
			vec_access(scope, __statement_offset(node));

		object_t *object = entry->object;

		if (is_assign_left) {
                        if (right_offset != -1) {
                                struct scope_entry *right = vec_access(scope, right_offset);
                                if ((__object_objattrs(object) & O_TYPE_UNDEFINED) ) {
                                        object->objattrs = right->object->objattrs | O_ATTR_MUTABLE;
					node->ref_attrs = object->objattrs;
                                }
                        }
                }
		node->ref_attrs = object->objattrs;
                return;
	}

	i = 0;

	if (__statement_kind(node) == __SEQUENCE_POINT__) {
		while (i < vec_size(__statement_children(node))) {
			while (i <
			       vec_size(__statement_children(node))) {
				eval_variable_flow(
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
	case EXPR_TYPE_VALUE:
		break;

	case EXPR_OP_TYPE_UNIOP:
		eval_variable_flow(__statement_left(node));
		break;

	case EXPR_OP_TYPE_BINOP:
		if (__statement_kind(node) == __ASSIGN__) {
		 	is_assign_left = false;
                        right_offset = -1;
                
			eval_variable_flow(
				__statement_right(node)
			);

                        is_assign_left = true;
                        right_offset = node->return_offset;

			eval_variable_flow(
				__statement_left(node)
			);
                
			is_assign_left = false;
                        right_offset = -1;

		 } else {
                        eval_variable_flow(
				__statement_left(node)
			);
                        eval_variable_flow(
				__statement_right(node)
			);
                }

		break;

	default:

		break;
	}
end:

	if (vec_size(node->frame)) {
		size_t i = vec_size(node->frame);
		while (i--) {
			vec_pop(scope, NULL);
		}
	}
}
