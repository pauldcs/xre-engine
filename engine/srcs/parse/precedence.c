#include "xre_assert.h"
#include "xre_parse.h"

int get_precedence_by_kind(xre_expr_kind_t kind)
{
	switch (kind) {
	case __BUILTIN_CALL__:
		switch (expr_type_by_kind(kind)) {
		case EXPR_OP_TYPE_BINOP:
			goto conditional;

		case EXPR_OP_TYPE_UNIOP:
			goto uniop;

		default:
			goto prison;
		}

	case __SCOPE_RESOLUTION__:
	case __START__:
	case __END__:
		return (0);

uniop:
	case __NOT__:
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
		return (-8);

	case __EQ__:
	case __NE__:
		return (-9);

	case __BAND__:
		return (-10);

	case __BXOR__:
		return (-11);

	case __BOR__:
		return (-12);

conditional:
	case __AND__:
	case __DO__:
	case __OR__:
	case __ELSE__:
		return (-13);

	case __LOOP__:
		return (-15);

	case __SEQUENCE__:
		return (-16);

	case __ASSIGN__:
		return (-18);

	case __SEPARATOR__:
		return (-19);

	case __LPAREN__:
	case __RPAREN__:
		return (-1000);

	case __VAL__:
	case __STRING_LITERAL__:
	case __VARIABLE__:
		break;
	}

prison:
	__return_val_if_fail__(false, 0);
	__builtin_unreachable();
}
