#include "xre_compiler.h"
#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_args.h"
#include "xre_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

static size_t __line	= 1;
static int    __counter = 0;
static int    __sp	= 0;
static size_t d		= 0;

static int __states[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

// enum operand {
//         D_DATA_STORE,
//         D_DATA_RELEASE,
//         D_DATA_LOAD,
//         P_ARITHMETHIC_ADD,
//         P_ARITHMETHIC_SUB,
//         P_ARITHMETHIC_MUL,
//         P_ARITHMETHIC_MOD,
//         P_ARITHMETHIC_DIV,
//         P_ARITHMETHIC_POW,
//         P_LOGIC_AND,
//         P_LOGIC_OR,
//         P_LOGIC_DO,
//         P_LOGIC_ELSE,
//         P_LOGIC_LOOP,
//         P_LOGIC_NOT,
//         P_BIT_AND,
//         P_BIT_OR,
//         P_BIT_XOR,
//         P_COMPARE_LT,
//         P_COMPARE_GT,
//         P_COMPARE_LE,
//         P_COMPARE_GE,
//         P_COMPARE_EQ,
//         P_COMPARE_NE,
// };

// struct instruction {
//         enum operand op;
//         bool tbit;

//         union {
//                 // process
//                 struct {
//                         int64_t sa; // source a
//                         int64_t sb; // source b
//                         int64_t dst; // destination
//                 };

//                 // data
//                 struct {
//                         int64_t sa; // source
//                         int64_t dst; // destination
//                 };

//                 // jump
//                 struct {
//                         int64_t sa; // source condition
//                         int64_t sqt; // consequence true
//                         int64_t sqf; // consequence false
//                 }
//         };
// };

void emit_ir(struct expression *node, bool verbose, bool is_left);

const char *tab[] = {
	"|-- ",
	"`-- ",
	"|   ",
	"    "
};

bool with_skip = false;
static void emit_code_internal(
	const char *code, bool lineno, bool indent, bool verbose, bool active, bool is_left
)
{
	(void)verbose;
	(void)is_left;
	(void)indent;
	if (lineno) {
		(void)fprintf(
			stdout, "%3lld: ", (unsigned long long)__line++
		);
	} else {
		(void)fprintf(stdout, "");
	}

	size_t i = 0;
		while (i < d + !active) {
			if (__states[i] == 1) {
				if (i + 1 == d + !active) {
					if (active) {
						if (__states[i] == 1) {
							printf("├── ");
						} else {
							if (with_skip) {
								printf("├── ");	
							} else {
								printf("└── ");
							}
						} 
					} else {
					}
				} else {
					printf("│   ");
				}
			} else {
				if (i + 1 == d + !active) {
					if (active) {
						if (__states[i] == 1) {
							printf("├── ");
						} else {
							if (with_skip) {
								printf("├── ");	
							} else {
								printf("└── ");
							}
						}  
					} else {
						printf("    ");	
					}
				} else {
					printf("    ");
				}
			}
			i++;
		}

	(void)fprintf(stdout, "%s\n", code);
	fflush(stdout);
}

static size_t frame_count_total_size(struct expression *node)
{
	size_t size = vec_size(__expression_locals(node));

	switch (__expression_type(node)) {
	case EXPR_OP_TYPE_UNIOP:
		return (size + frame_count_total_size(
				       __expression_binop_left(node)
			       ));

	case EXPR_OP_TYPE_BINOP:
		if (__expression_kind(node) == __SEQUENCE_POINT__) {
			size_t	       i = 0;
			struct vector *children =
				__expression_locals(node);
			while (i < vec_size(children)) {
				size += frame_count_total_size(
					vec_access(children, i++)
				);
			}
			return (size);
		}

		size += frame_count_total_size(
			__expression_binop_left(node)
		);
		size += frame_count_total_size(
			__expression_binop_right(node)
		);

		break;

	default:
		break;
	}

	return (size);
}

static void emit_builtin_call(struct expression *node, bool verbose, bool is_left)
{
	if (verbose) {
		emit_code_internal(
			format_string(
				"\"%s\" `%s` #%02d:%02d:%02d:%02d",
				expr_kind_to_string(
					__expression_kind(node)
				),
				node->builtin->iden,
				__expression_origin(node)->_line,
				__expression_origin(node)->_cols,
				__expression_origin(node)->_len,
				__expression_origin(node)->_depth
			),
			false,
			true,
			verbose,
			true,
			is_left
		);
	} else {
		emit_code_internal(
			format_string("\"%s\"", node->builtin->iden),
			false,
			true,
			verbose,
			true,
			is_left
		);
	}
}

static void
emit_basic_operation(struct expression *node, bool verbose, bool is_left)
{
	if (verbose) {
		emit_code_internal(
			format_string(
				"\"%s\" #%02d:%02d:%02d:%02d",
				expr_kind_to_string(
					__expression_kind(node)
				),
				__expression_origin(node)->_line,
				__expression_origin(node)->_cols,
				__expression_origin(node)->_len,
				__expression_origin(node)->_depth
			),
			false,
			true,
			verbose,
			true,
			is_left
		);
	} else {
		emit_code_internal(
			format_string(
				"\"%s\"",
				expr_kind_to_string(
					__expression_kind(node)
				)
			),
			false,
			true,
			verbose,
			true,
			is_left
		);
	}
}

static void emit_operation(struct expression *node, bool verbose, bool is_left)
{
	if (__expression_kind(node) == __BUILTIN_CALL__) {
		emit_builtin_call(node, verbose, is_left);
	} else {
		emit_basic_operation(node, verbose, is_left);
	}
}

static void emit_static_object(
	struct expression *node, object_t *object, bool verbose, bool is_left
)
{
	(void)node;
	with_skip = true;
	if (verbose) {
		emit_code_internal(
			format_string(
				"ALLOCATE [var_%04zu] as %s (0x%llx) #%s",
				++__counter - 1,
				object_attr_to_str(object->objattrs),
				__object_as_data(object),
				object->str
			),
			false,
			true,
			verbose,
			true,
			is_left
		);
	}
	with_skip = false;
}

static void emit_stack_free(struct expression *node, bool verbose, bool is_left)
{

	(void)node;
	if (verbose) {
		emit_code_internal(
			format_string(
				"DROP [var_%04zu]",
				__counter-- - 1
			),
			false,
			true,
			verbose,
			false,
			is_left
		);
	}
}

static void
emit_expression_sequence(struct expression *node, bool verbose, bool is_left)
{
	size_t	       i	= 0;
	struct vector *children = __expression_sequence(node);

	while (i < vec_size(children)) {
		if (i + 1 == vec_size(children)) {
			__states[d - 1] = 2;
		}
		emit_ir(vec_access(children, i++), verbose, is_left);
	}
}

static void emit_reference(struct expression *node, bool verbose, bool is_left)
{
	if (verbose) {
		emit_code_internal(
			format_string(
				"reference var_%04d as %s",
				__expression_ref_offset(node),
				object_attr_to_str(
					__expression_ref_access_mask(
						node
					)
				)
			),
			false,
			true,
			verbose,
			true,
			is_left
		);
	} else {
		emit_code_internal(
			format_string(
				"&%04d",
				__expression_ref_offset(node)
			),
			false,
			true,
			verbose,
			true,
			is_left
		);
	}
}

static void emit_frame_init(struct expression *node, bool verbose, bool is_left)
{
	with_skip = true;
	if (verbose) {
		emit_code_internal(
			format_string(
				"# VM space: %zu object(s) %02d:%02d:%02d:%02d",
				frame_count_total_size(node),
				__expression_origin(node)->_line,
				__expression_origin(node)->_cols,
				__expression_origin(node)->_len,
				__expression_origin(node)->_depth
			),
			false,
			true,
			verbose,
			true,
			is_left
		);
	}
	with_skip = false;
}

static void
handle_frame_change(struct expression *node, bool in, bool verbose, bool is_left)
{
	struct vector *frame	  = __expression_locals(node);
	size_t	       frame_size = vec_size(frame);
	size_t	       i	  = 0;

	if (in) {
		if (frame_size) {
			emit_frame_init(node, verbose, is_left);
			__sp += frame_size;

			while (i < frame_size) {
				emit_static_object(
					node,
					vec_access(frame, i++),
					verbose,
					is_left
				);
			}
		} else {
			/* no frame size */
			/* do nothing */
		}
	} else {
		__sp -= frame_size;
		while (i++ < frame_size) {
			emit_stack_free(node, verbose, is_left);
		}
	}
}

void emit_ir(struct expression *node, bool verbose, bool is_left)
{
	switch (__expression_type(node)) {
	case EXPR_TYPE_VALUE:
		emit_reference(node, verbose, is_left);
		return;

	case EXPR_OP_TYPE_UNIOP:
		emit_operation(node, verbose, is_left);
		__states[d] = 2;
		d++;
		handle_frame_change(node, true, verbose, false);
		emit_ir(__expression_binop_left(node), verbose, false);
		d--;
		__states[d] = 1;
		break;

	case EXPR_OP_TYPE_BINOP:
		emit_operation(node, verbose, is_left);
		__states[d] = 1;
		d++;
		handle_frame_change(node, true, verbose, is_left);

		if (__expression_kind(node) == __SEQUENCE_POINT__) {
			emit_expression_sequence(node, verbose, is_left);
			d--;
			handle_frame_change(node, false, verbose, is_left);
			break;
		}

		emit_ir(__expression_binop_left(node), verbose, true);
		__states[d - 1] = 2;
		emit_ir(__expression_binop_right(node), verbose, false);

		d--;
		__states[d] = 1;
		break;

	default:

		break;
	}

	with_skip = true;
	if (__expression_type(node) != EXPR_TYPE_VALUE) {
		if (__expression_dest_offset(node) != -1 && verbose) {
			emit_code_internal(
				format_string(
					"# Return offset [var_%04d]",
					__expression_dest_offset(node)
				),
				false,
				true,
				verbose,
				false,
				is_left
			);
		}
		if (__expression_dest_access_mask(node) !=
			    O_TYPE_UNDEFINED &&
		    verbose) {
			emit_code_internal(
				format_string(
					"# as %s",
					object_attr_to_str(
						__expression_dest_access_mask(
							node
						)
					)
				),
				false,
				true,
				verbose,
				false,
				is_left
			);	
		}
	}
	with_skip = false;
		handle_frame_change(node, false, verbose, is_left);
}
