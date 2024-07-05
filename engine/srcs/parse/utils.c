#include "xre_parse.h"
#include <sys/types.h>
#include <unistd.h>

static void inner(xre_ast_t *ast, size_t depth);

static void put_binop(xre_ast_t *ast, size_t depth)
{
	inner(ast->_binop.left, depth + 1);
	inner(ast->_binop.right, depth + 1);
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
	} else if (ast->kind == __IDENTIFIER__) {
		printf("id: '%s'\n", ast->string);
	} else {
		printf("<%s>\n", expr_kind_to_string(ast->kind));
	}

	if (ast->token._type & (EXPR_OP_TYPE_BINOP))
		put_binop(ast, depth);
	else if (ast->token._type & EXPR_OP_TYPE_UNIOP)
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
	case __IDENTIFIER__:
		free((void *)ast->string);
		break;

	case __BUILTIN_CALL__:
		//free((void *)ast->string);
		ast_free(ast->uniop);
		break;

	case __NOT__:
		ast_free(ast->uniop);
		break;

	case __VAL__:
		break;

	default:
		ast_free(ast->_binop.left);
		ast_free(ast->_binop.right);
	}
	free(ast);
}

const char *expr_kind_to_string(xre_expr_kind_t kind)
{
	switch (kind) {
	case __START__:
		return "start";
	case __END__:
		return "end";
	case __VAL__:
		return "value";
	case __STRING_LITERAL__:
		return "string_literal";
	case __IDENTIFIER__:
		return "identifier";
	case __NOT__:
		return "not";
	case __BUILTIN_CALL__:
		return "builtin_call";
	case __ADD__:
		return "addition";
	case __SUB__:
		return "substraction";
	case __MUL__:
		return "multiplication";
	case __DIV__:
		return "division";
	case __MOD__:
		return "modulus";
	case __LSHIFT__:
		return "left_shift";
	case __RSHIFT__:
		return "right_shift";
	case __ASSIGN__:
		return "assign";
	case __LT__:
		return "less_than";
	case __GT__:
		return "greater_than";
	case __LE__:
		return "less_or_eaqual";
	case __GE__:
		return "greater_than_or_equal";
	case __LPAREN__:
		return "left_parenthesis";
	case __RPAREN__:
		return "right_parenthesis";
	case __POW__:
		return "power";
	case __BXOR__:
		return "bitwise_xor";
	case __BAND__:
		return "bitwise_and";
	case __BOR__:
		return "bitwise_or";
	case __AND__:
		return "logical_and";
	case __OR__:
		return "logical_or";
	case __EQ__:
		return "equals";
	case __NE__:
		return "not_equal";
	case __SEQUENCE__:
		return "sequence_point";
	case __SEPARATOR__:
		return "separator";
	case __LOOP__:
		return "loop";
	case __DO__:
		return "do";
	case __ELSE__:
		return "else";
	}

	__builtin_unreachable();
}

xre_expr_type_t expr_type_by_kind(xre_expr_kind_t kind)
{
	switch (kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __IDENTIFIER__:
		return (EXPR_TYPE_VALUE);

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
	case __SEQUENCE__:
	case __LOOP__:
	case __DO__:
	case __ELSE__:
	case __AND__:
	case __OR__:

		return (EXPR_OP_TYPE_BINOP);

	case __NOT__:
	case __BUILTIN_CALL__:
		return (EXPR_OP_TYPE_UNIOP);

	case __START__:
	case __END__:
	case __LPAREN__:
	case __RPAREN__:
		return (EXPR_TYPE_OTHER);
	}

	__builtin_unreachable();
}
