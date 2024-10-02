#include "xre_compiler.h"
#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_args.h"
#include "xre_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#define BLK "\033[0;30m"
#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YEL "\033[0;33m"
#define BLU "\033[0;34m"
#define MAG "\033[0;35m"
#define CYN "\033[0;36m"
#define WHT "\033[0;37m"

//Regular bold text
#define BBLK "\033[1;30m"
#define BRED "\033[1;31m"
#define BGRN "\033[1;32m"
#define BYEL "\033[1;33m"
#define BBLU "\033[1;34m"
#define BMAG "\033[1;35m"
#define BCYN "\033[1;36m"
#define BWHT "\033[1;37m"

#define CRESET "\033[0m"

static size_t __line	= 1;
static int    __counter = 0;
static int    __sp	= 0;
static size_t d		= 0;

static int __states[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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

void emit_ir(__ast_node *node, bool is_left);

bool	    with_skip = false;
static void emit_code_internal(
	const char *code,
	bool	    lineno,
	bool	    indent,
	bool	    active,
	bool	    is_left
)
{
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
						printf(BLU
						       "├── " CRESET);
					} else {
						if (with_skip) {
							printf(BLU
							       "├── " CRESET
							);
						} else {
							printf(BLU
							       "└── " CRESET
							);
						}
					}
				} else {
				}
			} else {
				printf(BLU "│   " CRESET);
			}
		} else {
			if (i + 1 == d + !active) {
				if (active) {
					if (__states[i] == 1) {
						printf(BLU
						       "├── " CRESET);
					} else {
						if (with_skip) {
							printf(BLU
							       "├── " CRESET
							);
						} else {
							printf(BLU
							       "└── " CRESET
							);
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

static void emit_builtin_call(__ast_node *node, bool is_left)
{
	emit_code_internal(
		format_string(
			"%s-%s%s `%s` %s %s%s%s",
			BGRN,
			expr_kind_to_string(__expression_kind(node)),
			CRESET,
			node->builtin->iden,
			object_attr_to_str(
				__expression_dest_pmask(node)
			),
			MAG,
			node->dest.offset < 0 ?
				format_string(
					"local_%04d",
					node->dest.offset == -1 ?
						INT_MIN :
						-(node->dest.offset) -
							2
				) :
				format_string(
					"var_%04d", node->dest.offset
				),
			CRESET
		),
		false,
		true,
		true,
		is_left
	);
}

static void emit_basic_operation(__ast_node *node, bool is_left)
{
	emit_code_internal(
		format_string(
			"%s-%s%s %s %s%s%s",
			BGRN,
			expr_kind_to_string(__expression_kind(node)),
			CRESET,
			object_attr_to_str(
				__expression_dest_pmask(node)
			),
			MAG,
			node->dest.offset < 0 ?
				format_string(
					"local_%04d",
					node->dest.offset == -1 ?
						INT_MIN :
						-(node->dest.offset) -
							2
				) :
				format_string(
					"var_%04d", node->dest.offset
				),
			CRESET
		),
		false,
		true,
		true,
		is_left
	);
}

static void emit_operation(__ast_node *node, bool is_left)
{
	if (__expression_kind(node) == __BUILTIN_CALL__) {
		emit_builtin_call(node, is_left);
	} else {
		emit_basic_operation(node, is_left);
	}
}

static void
emit_static_object(__ast_node *node, object_t *object, bool is_left)
{
	(void)node;
	with_skip = true;
	emit_code_internal(
		format_string(
			"Alloc %s %svar_%04zu%s # (0x%llx) token: '%s'",
			object_attr_to_str(object->objattrs),
			BMAG,
			++__counter - 1,
			CRESET,
			__object_as_data(object),
			object->str
		),
		false,
		true,
		true,
		is_left
	);
	with_skip = false;
}

static void emit_stack_free(__ast_node *node, bool is_left)
{
	(void)node;
	emit_code_internal(
		format_string(
			"Drop %svar_%04zu%s",
			BMAG,
			__counter-- - 1,
			CRESET
		),
		false,
		true,
		false,
		is_left
	);
}

static void emit_expression_sequence(__ast_node *node, bool is_left)
{
	size_t	       i	= 0;
	struct vector *children = __expression_sequence(node);

	while (i < vec_size(children)) {
		if (i + 1 == vec_size(children)) {
			__states[d - 1] = 2;
		}
		emit_ir(vec_access(children, i++), is_left);
	}
}

static void emit_reference(__ast_node *node, bool is_left)
{
	emit_code_internal(
		format_string(
			"%s*%s%s %svar_%04d %s",
			CYN,
			CRESET,
			object_attr_to_str(__expression_ref_pmask(node
			)),
			BMAG,
			__expression_ref_offset(node),
			CRESET
		),
		false,
		true,
		true,
		is_left
	);
}

static void
handle_frame_change(__ast_node *node, bool in, bool is_left)
{
	struct vector *frame	  = __expression_frame_locals(node);
	size_t	       frame_size = vec_size(frame);
	size_t	       i	  = 0;

	if (in) {
		if (frame_size) {
			__sp += frame_size;

			while (i < frame_size) {
				emit_static_object(
					node,
					vec_access(frame, i++),
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
			emit_stack_free(node, is_left);
		}
	}
}

void emit_ir(__ast_node *node, bool is_left)
{
	switch (__expression_type(node)) {
	case EXPR_TYPE_VALUE:
		emit_reference(node, is_left);
		return;

	case EXPR_OP_TYPE_UNIOP:
		emit_operation(node, is_left);
		__states[d] = 2;
		d++;
		handle_frame_change(node, true, false);
		emit_ir(__expression_binop_left(node), false);
		d--;
		__states[d] = 1;
		break;

	case EXPR_OP_TYPE_BINOP:
		emit_operation(node, is_left);
		__states[d] = 1;
		d++;
		handle_frame_change(node, true, is_left);

		if (__expression_kind(node) == __SEQUENCE_POINT__) {
			emit_expression_sequence(node, is_left);
			d--;
			handle_frame_change(node, false, is_left);
			break;
		}

		emit_ir(__expression_binop_left(node), true);
		__states[d - 1] = 2;
		emit_ir(__expression_binop_right(node), false);

		d--;
		__states[d] = 1;
		break;

	default:

		break;
	}

	handle_frame_change(node, false, is_left);
}
