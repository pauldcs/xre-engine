#include "xre_parse.h"
#include "xre_assert.h"

int get_expr_precedence(xre_expr_kind_t kind)
{
	switch (kind) {
	case __SCOPE_RESOLUTION__:
	case __START__:
	case __END__:
		return (0);

	case __POW__:
		return (-1);

	case __INJECT__:
		return (-3);

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

	case __AND__:
	case __NOT__:
	case __DO__:
	case __OR__:
	case __ELSE__:
		return (-13);

	case __PRINT__:
	case __ASSIGN__:
		return (-14);

	case __LOOP__:
		return (-15);

	case __SEQUENCE__:
		return (-16);

	case __ANNOTATE__:
		return (-17);

	case __SEPARATOR__:
		return (-18);

	case __LPAREN__:
	case __RPAREN__:
		return (-1000);

	case __VAL__:
	case __STRING_LITERAL__:
	case __IDENTIFIER__:
		break;
	}

	__return_val_if_fail__(false, 0);
	__builtin_unreachable();
}