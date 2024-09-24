#include "xre_parse.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

static void inner(xre_ast_t *ast, size_t depth);

static void put_binop(xre_ast_t *ast, size_t depth)
{
	inner(ast->_binop.left, depth + 1);
	inner(ast->_binop.right, depth + 1);
}

static void put_sequence(xre_ast_t *ast, size_t depth)
{
	for (size_t c = 0; c < vec_size(ast->seq); c++) {
		inner((xre_ast_t *)vec_access(ast->seq, c),
		      depth + 1);
	}
}

static void put_uniop(xre_ast_t *ast, size_t depth)
{
	inner(ast->uniop, depth + 1);
}

static void inner(xre_ast_t *ast, size_t depth)
{
	size_t i;

	i = 0;
	printf("[%zu] ", ast->token._depth);
	while (i++ < depth)
		write(1, "   ", 3);

	if (!ast) {
		printf("!NULL!\n");
		return;
	}
	if (ast->kind == __VAL__) {
#ifdef __linux__
		printf("%ld\n", ast->value);
#else
		printf("%lld\n", ast->value);
#endif
	} else if (ast->kind == __STRING_LITERAL__) {
		printf("string: '%s'\n", ast->string);
	} else if (ast->kind == __VARIABLE__) {
		printf("id: '%s'\n", ast->string);
	} else {
		printf("<%s>\n", expr_kind_to_string(ast->kind));
	}

	if (ast->type & (EXPR_OP_TYPE_BINOP))
		put_binop(ast, depth);
	else if (ast->type & (EXPR_OP_TYPE_SEQUENCE)) {
		put_sequence(ast, depth);
	} else if (ast->type & EXPR_OP_TYPE_UNIOP)
		put_uniop(ast, depth);
}

void ast_show(xre_ast_t *ast)
{
	inner(ast, 0);
}

void ast_free(xre_ast_t *ast)
{
	if (!ast)
		return;

	switch (ast->kind) {
	case __STRING_LITERAL__:
	case __VARIABLE__:
		free((void *)ast->string);
		break;

	case __BUILTIN_CALL__:
		// free((void *)ast->string);
		ast_free(ast->uniop);
		break;

	case __NOT__:
		ast_free(ast->uniop);
		break;

	case __SEQUENCE__:
		break;

	case __VAL__:
		break;

	default:
		ast_free(ast->_binop.left);
		ast_free(ast->_binop.right);
	}
	free(ast);
}

const char *expr_type_to_string(xre_expr_type_t type)
{
	switch (type) {
	case EXPR_TYPE_VALUE:
		return ("value");
	case EXPR_OP_TYPE_SEQUENCE:
		return ("sequence");
	case EXPR_OP_TYPE_BINOP:
		return ("binop");
	case EXPR_OP_TYPE_UNIOP:
		return ("uniop");
	case EXPR_TYPE_OTHER:
		return ("other");
	}

	__builtin_unreachable();
}

const char *expr_kind_to_string(xre_expr_kind_t kind)
{
	switch (kind) {
	case __START__:
		return "Start";
	case __END__:
		return "End";
	case __VAL__:
		return "Value";
	case __STRING_LITERAL__:
		return "String literal";
	case __VARIABLE__:
		return "Variable";
	case __NOT__:
		return "Not";
	case __BUILTIN_CALL__:
		return "Builtin call";
	case __ADD__:
		return "Addition";
	case __SUB__:
		return "Substraction";
	case __MUL__:
		return "Multiplication";
	case __DIV__:
		return "Division";
	case __MOD__:
		return "Modulus";
	case __LSHIFT__:
		return "Left shift";
	case __RSHIFT__:
		return "Right shift";
	case __ASSIGN__:
		return "Assign";
	case __LT__:
		return "Less than";
	case __GT__:
		return "Greater than";
	case __LE__:
		return "Less or equal";
	case __GE__:
		return "Greater than or equal";
	case __LBRACK__:
		return "Left bracket";
	case __RBRACK__:
		return "Right bracket";
	case __LPAREN__:
		return "Left parenthesis";
	case __RPAREN__:
		return "Right parenthesis";
	case __POW__:
		return "Power";
	case __BXOR__:
		return "Bitwise xor";
	case __BAND__:
		return "Bitwise and";
	case __BOR__:
		return "Bitwise or";
	case __AND__:
		return "Logical and";
	case __OR__:
		return "Logical or";
	case __EQ__:
		return "Equals";
	case __CLOSURE__:
		return "Closure";
	case __NE__:
		return "Not equal";
	case __SEQUENCE_POINT__:
		return "Sequence";
	case __SEQUENCE__:
		return "Sequence";
	case __ATTRIBUTE__:
		return "Attribute";
	case __SEPARATOR__:
		return "Separator";
	case __LOOP__:
		return "Loop";
	case __SCOPE_RESOLUTION__:
		return "Scope resolution";
	case __DO__:
		return "Do";
	case __ELSE__:
		return "Else";
	default:
		printf("%d??\n", kind);
		return "";
	}

	__builtin_unreachable();
}

xre_expr_type_t expr_type_by_kind(xre_expr_kind_t kind)
{
	switch (kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __VARIABLE__:
		return (EXPR_TYPE_VALUE);

	case __SEQUENCE__:
		return (EXPR_OP_TYPE_SEQUENCE);

	case __ADD__:
	case __SUB__:
	case __MUL__:
	case __DIV__:
	case __MOD__:
	case __LSHIFT__:
	case __RSHIFT__:
	case __ASSIGN__:
	case __LT__:
	case __GT__:
	case __LE__:
	case __GE__:
	case __POW__:
	case __BXOR__:
	case __BAND__:
	case __BOR__:
	case __EQ__:
	case __NE__:
	case __SEPARATOR__:
	case __SEQUENCE_POINT__:
	case __ATTRIBUTE__:
	case __SCOPE_RESOLUTION__:
	case __LOOP__:
	case __DO__:
	case __ELSE__:
	case __AND__:
	case __OR__:
	case __CLOSURE__:
		return (EXPR_OP_TYPE_BINOP);

	case __NOT__:
	case __BUILTIN_CALL__:
		return (EXPR_OP_TYPE_UNIOP);

	case __START__:
	case __END__:
	case __LBRACK__:
	case __RBRACK__:
	case __LPAREN__:
	case __RPAREN__:
		return (EXPR_TYPE_OTHER);
	}

	__builtin_unreachable();
}
