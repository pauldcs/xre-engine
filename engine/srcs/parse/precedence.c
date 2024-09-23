#include "xre_compiler.h"
#include "xre_parse.h"

int get_precedence_by_kind(xre_expr_kind_t kind)
{
	// if (kind == __BUILTIN_CALL__) {
	// 	switch (expr_type_by_kind(kind)) {
	// 		case EXPR_OP_TYPE_BINOP:
	// 			goto builtin_binop;

	// 		case EXPR_OP_TYPE_UNIOP:
	// 			goto uniop;

	// 		default:
	// 			goto prison;
	// 	}
	// }

	switch (kind) {
	case __SCOPE_RESOLUTION__:
	case __START__:
	case __END__:
		return (0);

	case __NOT__:
	case __ATTRIBUTE__:
	case __BUILTIN_CALL__:
		return (-1);

	case __POW__:
		return (-2);

	case __MUL__:
	case __DIV__:
	case __MOD__:
		return (-4);

	case __ADD__:
	case __SUB__:
		return (-5);

	case __LSHIFT__:
	case __RSHIFT__:
		return (-6);

	case __LT__:
	case __GT__:
	case __LE__:
	case __GE__:
		return (-7);

	case __EQ__:
	case __NE__:
		return (-8);

	case __BAND__:
		return (-9);

	case __BXOR__:
		return (-10);

	case __BOR__:
		return (-11);

	case __AND__:
		return (-13);

	case __OR__:
		return (-14);

	case __DO__:
		return (-15);

	case __ELSE__:
		return (-16);

	case __LOOP__:
	case __SEQUENCE__:
		return (-17);

	case __SEQUENCE_POINT__:
		return (-18);

	case __ASSIGN__:
		return (-19);

	case __CLOSURE__:
		return (-21);

	case __SEPARATOR__:
		return (-22);

	case __LBRACK__:
	case __RBRACK__:
	case __LPAREN__:
	case __RPAREN__:
		return (-1000);

	case __VAL__:
	case __STRING_LITERAL__:
	case __VARIABLE__:
	default:

		break;
	}

	__return_val_if_fail__(false, 0);
	__builtin_unreachable();
}
