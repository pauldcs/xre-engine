#include "xre_parse.h"
#include <sys/types.h>
#include <unistd.h>

static void	inner(t_xre_ast *ast, size_t depth);

static void
put_binop (t_xre_ast *ast, size_t depth) {

	inner(ast->binop.left, depth + 1);
	inner(ast->binop.right, depth + 1);
}

static void
put_uniop (t_xre_ast *ast, size_t depth) {

	inner(ast->uniop, depth + 1);
}

static void
inner (t_xre_ast *ast, size_t depth) {
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

	if (ast->token._type & (EXPR_OP_TYPE_BINOP | EXPR_TYPE_CONDITION | EXPR_TYPE_LOOP |EXPR_TYPE_SEQUENCE))
		put_binop(ast, depth);
	else if (ast->token._type & EXPR_OP_TYPE_UNIOP)
		put_uniop(ast, depth);
}

void
ast_show (t_xre_ast *ast) {
	inner(ast, 0);
}

const char *
expr_kind_to_string(t_xre_expr_kind kind) {
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
    case __OR_ASSIGN__:        return "assign or";
    case __AND_ASSIGN__:       return "assign and";
    case __LSHIFT_ASSIGN__:    return "lshift assign";
    case __RSHIFT_ASSIGN__:    return "rshift assign";
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
    case __INJECT__:           return "injection";
    case __ANNOTATE__:         return "annotation";
    case __LOOP__:             return "loop";
    case __DO__:               return "do";
    case __ELSE__:             return "else";
    case __SCOPE_RESOLUTION__: return "scope resolution";
    }
    return "????????";
}

t_xre_expr_type
expr_type_by_kind(t_xre_expr_kind kind) {
    switch (kind) {
        case __VAL__:              
        case __STRING_LITERAL__:   
        case __IDENTIFIER__:       
            return (EXPR_TYPE_OPERAND);

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
        case __OR_ASSIGN__:        
        case __AND_ASSIGN__:       
        case __LSHIFT_ASSIGN__:    
        case __RSHIFT_ASSIGN__:         
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
            return (EXPR_OP_TYPE_BINOP);
        
        case __SEQUENCE_POINT__:
            return (EXPR_TYPE_SEQUENCE); 

        case __INJECT__:
            return (EXPR_TYPE_INJECT); 

        case __LOOP__:             
            return (EXPR_TYPE_LOOP);
        
        case __DO__:             
        case __ELSE__:             
        case __AND__:              
        case __OR__:               
            return (EXPR_TYPE_CONDITION);
        
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