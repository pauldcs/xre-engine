#include "xre_compiler.h"
#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_args.h"
#include "xre_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

//#define SYNTAX_OFF

static bool _isatty = false;

__attribute__((constructor)) void isa_tty(void)
{
	_isatty =
		(tcgetpgrp(STDOUT_FILENO) > 0 ||
		 tcgetpgrp(STDERR_FILENO) > 0);
}

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

static int    __counter = 0;
static int    __sp	= 0;
static size_t d		= 0;

static int __states[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

const char *tty_convert_esc(const char *escape)
{
	if (_isatty) {
		return (escape);
	}
	return ("");
}

void emit_ir(struct expression *node, bool is_left);

const char *expression_kind_string(enum expression_kind kind)
{
	switch (kind) {
	case ARITHMETHIC_ADD:
		return ("Arithmetic Add");

	case ARITHMETHIC_SUB:
		return ("Arithmetic Sub");

	case ARITHMETHIC_MUL:
		return ("Arithmetic Mul");

	case ARITHMETHIC_MOD:
		return ("Arithmetic Mod");

	case ARITHMETHIC_DIV:
		return ("Arithmetic Div");

	case ARITHMETHIC_POW:
		return ("Pow");

	case LOGIC_AND:
		return ("Logical And");

	case LOGIC_OR:
		return ("Logical Or");

	case LOGIC_DO:
		return ("Logical Do");

	case LOGIC_ELSE:
		return ("Logical Else");

	case LOGIC_LOOP:
		return ("Logical Loop");

	case LOGIC_NOT:
		return ("Logical Not");

	case BIT_AND:
		return ("Bitwise And");

	case BIT_OR:
		return ("Bitwise Or");

	case BIT_XOR:
		return ("Bitwise Xor");

	case BIT_LSHIFT:
		return ("Bitwise Lshift");

	case BIT_RSHIFT:
		return ("Bitwise Rshift");

	case COMPARE_LT:
		return ("Compare LT");

	case COMPARE_GT:
		return ("Compare GT");

	case COMPARE_LE:
		return ("Compare LE");

	case COMPARE_GE:
		return ("Compare GE");

	case COMPARE_EQ:
		return ("Compare EQ");

	case COMPARE_NE:
		return ("Compare NE");

	case REFERENCE:
		return ("Reference");

	case SEQUENCE:
		return ("Sequence");

	case CLOSURE:
		return ("Closure");

	case ASSIGN:
		return ("Assign");

	case SEPARATOR:
		return ("Separator");

	case BUILTIN_CALL:
		return ("Builtin Call");

	case IMPOSSIBLE:
	default:
		return ("Impossible");
	};
}

bool with_skip = false;
static void
emit_code_internal(const char *code, bool active, bool is_left)
{
	(void)is_left;
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

static const char *pointer_string(struct pointer *pointer)
{
	const char *ret_type   = "??type";
	const char *ret_prot   = "??protection";
	const char *ret_offset = "??offset";

	if (__pointer_known_offset(*pointer)) {
		if (pointer->offset < 0) {
			ret_offset = format_string(
				"%s[local_%04lld]%s",
				MAG,
				-pointer->offset,
				CRESET
			);
		} else {
			ret_offset = format_string(
				"%s[var_%04lld]%s",
				BMAG,
				pointer->offset,
				CRESET
			);
		}
	}

	if (__pointer_known_port(*pointer)) {
		ret_type =
			port_type_string(__pointer_port(*pointer).type
			);
		ret_prot =
			port_prot_string(__pointer_port(*pointer).protection
			);
	}

	return (format_string(
		"%s%s%s %s%s%s %s",
		"",
		ret_prot,
		CRESET,
		BCYN,
		ret_type,
		CRESET,
		ret_offset
	));
}

static void emit_builtin_call(struct expression *node, bool is_left)
{
	const char *kind_str =
		expression_kind_string(__node_attr_kind(node));
	const char *builtin_name = __node_builtin(node)->iden;

	struct pointer pointer = __node_pointer(node);

	const char *format = format_string(
		"%s%s%s `%s` %s",
		BGRN,
		kind_str,
		CRESET,
		builtin_name,
		pointer_string(&pointer)
	);

	emit_code_internal(format, true, is_left);
}

static void emit_basic_operation(struct expression *node, bool is_left)
{
	const char *kind_str =
		expression_kind_string(__node_attr_kind(node));

	struct pointer pointer = __node_pointer(node);

	const char *format = format_string(
		"%s%s%s %s",
		BGRN,
		kind_str,
		CRESET,
		pointer_string(&pointer)
	);

	emit_code_internal(format, true, is_left);
}

static void emit_operation(struct expression *node, bool is_left)
{
	if (__node_token_kind(node) == __BUILTIN_CALL__) {
		emit_builtin_call(node, is_left);
	} else {
		emit_basic_operation(node, is_left);
	}
}

static void emit_static_object(object_t *object, bool is_left)
{
	with_skip = true;
	const char *object_type =
		port_type_string(__object_port(object).type);

	const char *object_prot =
		port_prot_string(__object_port(object).protection);

	const char *format = format_string(
		"%sreserve%s %s%s%s %s%s%s %svar_%04zu%s # token: '%s'",
		GRN,
		CRESET,
		BCYN,
		object_type,
		CRESET,
		"",
		object_prot,
		CRESET,
		BWHT,
		++__counter - 1,
		CRESET,
		object->str
	);

	emit_code_internal(format, true, is_left);
	with_skip = false;
}

static void emit_stack_free(void)
{
	const char *format = format_string(
		"%sdrop%s var_%04zu", RED, CRESET, __counter-- - 1
	);

	emit_code_internal(format, false, false);
}

static void emit_expression_sequence(struct expression *node, bool is_left)
{
	size_t	       i	= 0;
	struct vector *children = __node_as_sequence(node);

	while (i < vec_size(children)) {
		if (i + 1 == vec_size(children)) {
			__states[d - 1] = 2;
		}
		emit_ir(vec_access(children, i++), is_left);
	}
}

static void emit_reference(struct expression *node, bool is_left)
{
	struct pointer pointer = __node_as_reference(node);

	const char *format = format_string(
		"%s&%s%s", BRED, CRESET, pointer_string(&pointer)
	);

	emit_code_internal(format, true, is_left);
}

static void
handle_frame_change(struct expression *node, bool in, bool is_left)
{
	struct vector *frame	  = __node_locals(node);
	size_t	       frame_size = vec_size(frame);
	size_t	       i	  = 0;

	if (in) {
		if (frame_size) {
			__sp += frame_size;
			while (i < frame_size) {
				emit_static_object(
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
			emit_stack_free();
		}
	}
}

void emit_ir(struct expression *node, bool is_left)
{
	switch (__node_token_type(node)) {
	case EXPR_TYPE_VALUE:
		emit_reference(node, is_left);
		return;

	case EXPR_OP_TYPE_UNIOP:
		emit_operation(node, is_left);
		__states[d] = 2;
		d++;
		handle_frame_change(node, true, false);
		emit_ir(__node_as_binop_l(node), false);
		d--;
		__states[d] = 1;
		break;

	case EXPR_OP_TYPE_BINOP:
		emit_operation(node, is_left);
		__states[d] = 1;
		d++;
		handle_frame_change(node, true, is_left);

		if (__node_token_kind(node) == __SEQUENCE_POINT__) {
			emit_expression_sequence(node, is_left);
			d--;
			handle_frame_change(node, false, is_left);
			break;
		}

		emit_ir(__node_as_binop_l(node), true);
		__states[d - 1] = 2;
		emit_ir(__node_as_binop_r(node), false);

		d--;
		__states[d] = 1;
		break;

	default:

		break;
	}

	handle_frame_change(node, false, is_left);
}
