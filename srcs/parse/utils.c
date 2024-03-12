#include "xre_parse.h"
#include <sys/types.h>
#include <unistd.h>

static void	inner(xre_ast_t *ast, size_t depth);

static void
put_binop (xre_ast_t *ast, size_t depth) {

	inner(ast->_binop.left, depth + 1);
	inner(ast->_binop.right, depth + 1);
}

static void
put_uniop (xre_ast_t *ast, size_t depth) {

	inner(ast->uniop, depth + 1);
}

static void
inner (xre_ast_t *ast, size_t depth) {
	size_t	i;

	i = 0;
	while (i++ < depth)
		write(1, "   ", 3);

  if (!ast) {
    printf("!NULL!\n");
		return ;
	}
  if (ast->kind == __VAL__) {
#ifdef __linux__
    printf("> %ld\n", ast->value);
#else
    printf("> %lld\n", ast->value);
#endif
  }else if (ast->kind == __STRING_LITERAL__) {
    printf("> string: '%s'\n", ast->string);
  }
  else if (ast->kind == __IDENTIFIER__) {
	printf("> id: '%s'\n", ast->string);
} else {
    write(1, "> ", 2);
	  printf("[%s]\n", expr_kind_to_string(ast->kind));
  }

	if (ast->token._type & (EXPR_OP_TYPE_BINOP))
		put_binop(ast, depth);
	else if (ast->token._type & EXPR_OP_TYPE_UNIOP)
		put_uniop(ast, depth);
}

void
ast_show (xre_ast_t *ast) {
	inner(ast, 0);
}

void
ast_free(xre_ast_t *ast) {

    if (!ast)
        return ;
    
    switch(ast->kind) {
        case __STRING_LITERAL__:
        case __IDENTIFIER__:
            free((void *)ast->string);
            break;

        case __NOT__:
            ast_free(ast->uniop);
            break;

        case __VAL__:
            break;

        default:
            break;
            ast_free(ast->_binop.left);
            ast_free(ast->_binop.right);          
    }
    free(ast);
}

const char *
expr_kind_to_string(xre_expr_kind_t kind) {
    switch (kind) {
    case __START__:            return "start";
    case __END__:              return "end";
    case __VAL__:              return "value";
    case __STRING_LITERAL__:   return "string literal";
    case __IDENTIFIER__:       return "identifier";
    case __NOT__:              return "not";
    case __ADD__:              return "addition";
    case __SUB__:              return "substraction";
    case __MUL__:              return "multiplication";
    case __DIV__:              return "division";
    case __MOD__:              return "modulus";
    case __LSHIFT__:           return "left shift";
    case __RSHIFT__:           return "right shift";
    case __ASSIGN__:           return "assign";
    case __ADD_ASSIGN__:       return "assign add";
    case __SUB_ASSIGN__:       return "assign sub";
    case __DIV_ASSIGN__:       return "assign div";
    case __MUL_ASSIGN__:       return "assign mul";
    case __MOD_ASSIGN__:       return "assign mod";
    case __POW_ASSIGN__:       return "assign pow";
    case __LT__:               return "less than";
    case __GT__:               return "greater than";
    case __LE__:               return "less or eaqual";
    case __GE__:               return "greater than or equal";
    case __LPAREN__:           return "left parenthesis";
    case __RPAREN__:           return "right parenthesis";
    case __POW__:              return "power";
    case __BXOR__:             return "bitwise xor";
    case __BAND__:             return "bitwise and";
    case __BOR__:              return "bitwise or";
    case __AND__:              return "logical and";
    case __OR__:               return "logical or";
    case __EQ__:               return "equals";
    case __NE__:               return "not equal";
    case __SEQUENCE_POINT__:   return "sequence point";
    case __SEPARATOR__:        return "separator";
    case __INJECT__:           return "injection";
    case __ANNOTATE__:         return "annotation";
    case __LOOP__:             return "loop";
    case __DO__:               return "do";
    case __ELSE__:             return "else";
    case __SCOPE_RESOLUTION__: return "scope resolution";
    }
    return "????????";
}

xre_expr_type_t
expr_type_by_kind(xre_expr_kind_t kind) {
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
        case __ADD_ASSIGN__:       
        case __SUB_ASSIGN__:       
        case __DIV_ASSIGN__:       
        case __MUL_ASSIGN__:       
        case __MOD_ASSIGN__:       
        case __POW_ASSIGN__:              
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
        case __ANNOTATE__:         
        case __SCOPE_RESOLUTION__:
        case __SEPARATOR__:
        case __SEQUENCE_POINT__:
        case __INJECT__:
        case __LOOP__:             
        case __DO__:             
        case __ELSE__:             
        case __AND__:              
        case __OR__:               
        
            return (EXPR_OP_TYPE_BINOP);
        
        case __NOT__:
            return (EXPR_OP_TYPE_UNIOP);

        case __START__:            
        case __END__:              
        case __LPAREN__:           
        case __RPAREN__:           
            return (EXPR_TYPE_OTHER);
    }
    return (-1000);
}
