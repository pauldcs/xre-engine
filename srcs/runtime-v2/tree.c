#include "xre_assert.h"
#include "xre_runtime.h"
#include "xre_operations.h"
#include <string.h>

static op_info_t oper_lookup_table[] = {
	[__ASSIGN__] = {
		//.type = STMT_ASSIGN,
		.fptr = (void *)oper_assign,
		.init = assignment_initiator,
	},
	[__ADD_ASSIGN__] = {
		//.type = STMT_ADD_ASSIGN,
		.fptr = (void *)oper_add_assign,
		.init = assignment_initiator,
	},
	[__SUB_ASSIGN__] = {
		//.type = STMT_SUB_ASSIGN,
		.fptr = (void *)oper_sub_assign,
		.init = assignment_initiator,
	},
	[__DIV_ASSIGN__] = {
		//.type = STMT_DIV_ASSIGN,
		.fptr = (void *)oper_div_assign,
		.init = assignment_initiator,
	},
	[__MOD_ASSIGN__] = {
		//.type = STMT_MOD_ASSIGN,
		.fptr = (void *)oper_mod_assign,
		.init = assignment_initiator,
	},
	[__POW_ASSIGN__] = {
		//.type = STMT_POW_ASSIGN,
		.fptr = (void *)oper_pow_assign,
		.init = assignment_initiator,
	},
	[__MUL_ASSIGN__] = {
		//.type = STMT_MUL_ASSIGN,
		.fptr = (void *)oper_mul_assign,
		.init = assignment_initiator,
	},
	[__ADD__] = {
		//.type = STMT_ADD,
		.fptr = (void *)oper_add,
		.init = arithmetic_initiator,
	},
	[__SUB__] = {
		//.type = STMT_SUB,
		.fptr = (void *)oper_sub,
		.init = arithmetic_initiator,
	},
	[__MUL__] = {
		//.type = STMT_MUL,
		.fptr = (void *)oper_mul,
		.init = arithmetic_initiator,
	},
	[__DIV__] = {
		//.type = STMT_DIV,
		.fptr = (void *)oper_div,
		.init = arithmetic_initiator,
	},
	[__MOD__] = {
		//.type = STMT_MOD,
		.fptr = (void *)oper_mod,
		.init = arithmetic_initiator,
	},
	[__POW__] = {
		//.type = STMT_POW,
		.fptr = (void *)oper_pow,
		.init = arithmetic_initiator,
	},
	[__BAND__] = {
		//.type = STMT_BAND,
		.fptr = (void *)oper_bw_and,
		.init = arithmetic_initiator,
	},
	[__BOR__] = {
		//.type = STMT_BOR,
		.fptr = (void *)oper_bw_or,
		.init = arithmetic_initiator,
	},
	[__BXOR__] = {
		//.type = STMT_BXOR,
		.fptr = (void *)oper_bw_xor,
		.init = arithmetic_initiator,
	},
	[__LSHIFT__] = {
		//.type = STMT_LSHIFT,
		.fptr = (void *)oper_lshift,
		.init = arithmetic_initiator,
	},
	[__RSHIFT__] = {
		//.type = STMT_RSHIFT,
		.fptr = (void *)oper_rshift,
		.init = arithmetic_initiator,
	},
	[__EQ__] = {
		//.type = STMT_EQ,
		.fptr = (void *)oper_eq,
		.init = relational_initiator,
	},
	[__NE__] = {
		//.type = STMT_NE,
		.fptr = (void *)oper_ne,
		.init = relational_initiator,
	},
	[__LT__] = {
		//.type = STMT_LT,
		.fptr = (void *)oper_lt,
		.init = relational_initiator,
	},
	[__GT__] = {
		//.type = STMT_GT,
		.fptr = (void *)oper_gt,
		.init = relational_initiator,
	},
	[__LE__] = {
		//.type = STMT_LE,
		.fptr = (void *)oper_le,
		.init = relational_initiator,
	},
	[__GE__] = {
		//.type = STMT_GE,
		.fptr = (void *)oper_ge,
		.init = relational_initiator,
	},
	[__NOT__] = {
		//.type = STMT_NOT,
		.fptr = (void *)oper_not,
		.init = relational_initiator,
	},
	[__LOOP__] = {
		//.type = STMT_LOOP,
		.fptr = (void *)oper_loop,
		.init = logical_initiator,
	},
	[__DO__] = {
		//.type = STMT_DO,
		.fptr = (void *)oper_do,
		.init = logical_initiator,
	},
	[__ELSE__] = {
		//.type = STMT_ELSE,
		.fptr = (void *)oper_else,
		.init = logical_initiator,
	},
	[__AND__] = {
		//.type = STMT_AND,
		.fptr = (void *)oper_and,
		.init = logical_initiator,
	},
	[__OR__] = {
		//.type = STMT_OR,
		.fptr = (void *)oper_or,
		.init = logical_initiator,
	},
	[__IDENTIFIER__] = {
		//.type = STMT_IDENTIFIER,
		.fptr = NULL,
		.init = value_initiator,
	},
	[__VAL__] = {
		//.type = STMT_VAL,
		.fptr = NULL,
		.init = value_initiator,
	},
	[__STRING_LITERAL__] = {
		//.type = STMT_STRING_LITERAL,
		.fptr = NULL,
		.init = value_initiator,
	}
	// [__SEQUENCE_POINT__] = {
	// 	.type = STMT_SEQUENCE_POINT,
	// 	.fptr = oper_sequence_point,
	// },
	// [__SEPARATOR__] = {
	// 	.type = STMT_SEPARATOR,
	// 	.fptr = oper_separator,
	// },
	// [__MEMBER__] = {
	// 	.type = STMT_MEMBER,
	// 	.fptr = oper_member,
	// },
};

static stmt_tree_t *alloc_stmt_tree(size_t size)
{
	size_t alloc_size = sizeof(stmt_tree_t) * size;
	stmt_tree_t *tree = malloc(alloc_size);
	if (!tree) {
		XRE_LOGGER(error, "Out of memory");
		exit(1);
	}
	(void)memset(tree, 0x00, alloc_size);
	return (tree);
}

static size_t get_tree_size(xre_ast_t *ast)
{
	size_t size = 1;

	switch (ast->kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __IDENTIFIER__:
		return (size);

	case __NOT__:
	case __PRINT__:
		return (size + get_tree_size(ast->_binop.left));

	default:
		return (size + get_tree_size(ast->_binop.left) +
			get_tree_size(ast->_binop.right));
	}
}

static int stmt_tree_create(stmt_tree_t *stmt, xre_ast_t *ast)
{
	static int index = 0;

	stmt_tree_t *current = stmt + index;
	int initial_index = index++;

	current->orig = (xre_token_t *)&ast->token;
	current->op = oper_lookup_table[ast->kind];

	switch (ast->kind) {
	case __VAL__:
		current->value = ast->value;
		break;

	case __STRING_LITERAL__:
	case __IDENTIFIER__:
		current->string = (char *)ast->string;
		break;

	case __NOT__:
	case __PRINT__:
		current->br.left = stmt_tree_create(stmt, ast->uniop);
		current->br.right = -1;
		break;

	default:
		current->br.left = stmt_tree_create(stmt, ast->_binop.left);
		current->br.right = stmt_tree_create(stmt, ast->_binop.left);
		break;
	}

	return (initial_index);
}

stmt_tree_t *stmt_tree_init(xre_ast_t *ast)
{
	size_t tree_size = get_tree_size(ast);

	stmt_tree_t *stmt = alloc_stmt_tree(tree_size);
	if (!stmt) {
		return (NULL);
	}
	(void)stmt_tree_create(stmt, ast);
	return (stmt);
}

void stmt_tree_fini(stmt_tree_t *tree)
{
	free(tree);
}
