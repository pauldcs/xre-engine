#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdbool.h>

/* DEFINE GLOBAL */
struct vector *scope = NULL;

/* DEFINE GLOBAL */
int64_t assign_next_ref = false;
int64_t save_next_ref = false;

/* DEFINE GLOBAL */
static struct port *assign_src;

static void analyzer(struct expression *node)
{
	size_t i = 0;
	while (i < vec_size(__node_locals(node))) {
		object_t *op = (object_t *)vec_access(
			__node_locals(node), i
		);

		(void)vec_push(scope, &op);
		i++;
	}

	if (__node_attr_kind(node) == REFERENCE) {
		object_t *this_object = *(object_t **)vec_access(
			scope,
			__pointer_offset(__node_as_reference(node))
		);

		__node_as_reference(node).port = this_object->port;

		if (assign_next_ref && assign_src) {
			struct port *dst_port = &this_object->port;
			struct port *src_port = assign_src;

			//printf("dst_port %s %s\n", port_type_string(dst_port->type), port_prot_string(dst_port->protection));
			//printf("src_port %s %s\n", port_type_string(src_port->type), port_prot_string(src_port->protection));
			
			//(void)memcpy(dst_port, src_port, sizeof(struct port));

			if (dst_port->type == UNDEFINED) {
				dst_port->protection = RDWR;
				dst_port->type = src_port->type;
			}
			return;
		} else {
			//printf("%s %s\n", port_type_string(this_object->port.type), port_prot_string(this_object->port.prot));
		}

		if (save_next_ref) {
			assign_src = &this_object->port;
		}
		return;
	}

	i = 0;

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
		analyzer(__node_as_binop_l(node));
		break;

	case EXPR_OP_TYPE_BINOP:
		if (__node_token_kind(node) == __ASSIGN__) {
			assign_next_ref	= false;
			save_next_ref = true;
		} else {
			assign_next_ref	 = false;
			save_next_ref = false;
		}

		analyzer(__node_as_binop_l(node));

		if (__node_token_kind(node) == __ASSIGN__) {
			assign_next_ref	= true;
			save_next_ref = false;
		} else {
			assign_next_ref	   = false;
			save_next_ref = false;
		}

		analyzer(__node_as_binop_r(node));

		if (__node_token_kind(node) == __ASSIGN__) {
			assign_next_ref	   = false;
			save_next_ref = false;
			assign_src = NULL;
		}

		break;

	default:

		break;
	}
end:

	if (vec_size(__node_locals(node))) {
		size_t i = vec_size(__node_locals(node));
		while (i--) {
			vec_pop(scope, NULL);
		}
	}
}

bool determine_variable_properties(struct expression *node)
{
	scope = vec_create(sizeof(object_t *), 64, NULL);
	analyzer(node);
	vec_kill(scope);
	return (true);
}
