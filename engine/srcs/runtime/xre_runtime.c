#include "xre_runtime.h"
#include "xre_compiler.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include "xre_args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

err_notif_t _error;
struct statement *__statements__ = NULL;

typedef bool (*runtime_op_t)(struct statement *);


static runtime_op_t oper_lookup_table[] = {
	[__ASSIGN__]	       = oper_assign,
	[__ADD__]	       = oper_add,
	[__SUB__]	       = oper_sub,
	[__MUL__]	       = oper_mul,
	[__DIV__]	       = oper_div,
	[__MOD__]	       = oper_mod,
	[__POW__]	       = oper_pow,
	[__BAND__]	       = oper_bw_and,
	[__BOR__]	       = oper_bw_or,
	[__BXOR__]	       = oper_bw_xor,
	[__LSHIFT__]	       = oper_lshift,
	[__RSHIFT__]	       = oper_rshift,
	[__EQ__]	       = oper_eq,
	[__CLOSURE__]	       = oper_closure,
	[__NE__]	       = oper_ne,
	[__LT__]	       = oper_lt,
	[__GT__]	       = oper_gt,
	[__LE__]	       = oper_le,
	[__GE__]	       = oper_ge,
	[__NOT__]	       = oper_not,
	[__LOOP__]	       = oper_loop,
	[__SCOPE_RESOLUTION__] = oper_scope_resolution,
	[__DO__]	       = oper_do,
	[__ELSE__]	       = oper_else,
	[__AND__]	       = oper_and,
	[__OR__]	       = oper_or,
	[__VARIABLE__]	       = oper_symbol,
	[__VAL__]	       = oper_value,
	[__STRING_LITERAL__]   = oper_string,
	[__SEQUENCE__]	       = oper_sequence,
	[__ATTRIBUTE__]	       = oper_attribute,
	[__SEPARATOR__]	       = oper_separator,
};

static bool alloc_zeroed_stmt_tree(size_t size, struct statement**buffer)
{
	size_t alloc_size = sizeof(struct statement) * size;
	*buffer		  = malloc(alloc_size);
	if (!*buffer) {
		return (false);
	}

	(void)memset(*buffer, 0x00, alloc_size);
	return (true);
}

static size_t get_tree_size(xre_ast_t *ast)
{
	size_t size = 1;

	switch (ast->kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __VARIABLE__:
		return (size);

	case __SEQUENCE__:
		return (size + vec_size(ast->seq));

	case __NOT__:
	case __BUILTIN_CALL__:
		if (ast->type == EXPR_TYPE_VALUE) {
			return (size);
		}

		if (ast->type == EXPR_OP_TYPE_UNIOP) {
			return (size + get_tree_size(ast->_binop.left)
			);
		}

		if (ast->type == EXPR_OP_TYPE_BINOP) {
			// fallthrough
		}

	default:

		return (size + get_tree_size(ast->_binop.left) +
			get_tree_size(ast->_binop.right));
	}
}

static int
stmt_tree_init(struct statement *stmt, xre_ast_t *ast, bool reset_index)
{
	static int index = 0;
	if (reset_index) {
		index = 0;
	}

	struct statement *current	  = stmt + index;
	int	    n		  = 0;
	int	    tmp		  = 0;
	int	    initial_index = index++;


	current->orig = (xre_token_t *)&ast->token;
	current->local = vec_create(sizeof(char *), 3, NULL);

	if (ast->kind == __BUILTIN_CALL__) {
		current->eval = get_builtin_ptr(
			ast->token._ptr, ast->token._len
		);
	} else {
		current->eval = oper_lookup_table[ast->kind];
	}

	switch (ast->kind) {
	case __VAL__:
		current->value = ast->value;
		break;

	case __STRING_LITERAL__:
		current->string = (char *)ast->string;
		break;

	case __VARIABLE__:
		current->string = (char *)ast->string;
		break;

	case __SEQUENCE__:
		n		  = vec_size(ast->seq);
		current->children = vec_create(sizeof(int), n, NULL);
		for (int i = 0; i < n; i++) {
			xre_ast_t *a = vec_access(ast->seq, i);
			tmp	     = stmt_tree_init(stmt, a, false);
			vec_push(current->children, &tmp);
		}
		break;

	case __NOT__:
	case __BUILTIN_CALL__:
		if (ast->type == EXPR_TYPE_VALUE) {
			break;
		}

		if (ast->type == EXPR_OP_TYPE_UNIOP) {
			current->br.left = stmt_tree_init(
				stmt, ast->uniop, false
			);
			current->br.right = -1;
			break;
		}

		if (ast->type == EXPR_OP_TYPE_BINOP) {
			goto binop;
		}
		break;

	case __CLOSURE__:
		// current->symbols =
		// 	vec_create(sizeof(object_t), 3, NULL);
		// fallthrough

binop:
	default:
		current->br.left =
			stmt_tree_init(stmt, ast->_binop.left, false);
		current->br.right = stmt_tree_init(
			stmt, ast->_binop.right, false
		);
		break;
	}

	return (initial_index);
}

static bool stmt_tree_create(xre_ast_t *ast, struct statement **buffer)
{
	if (!alloc_zeroed_stmt_tree(
		    get_tree_size(ast) * sizeof(struct statement), buffer
	    )) {
		return (false);
	}

	(void)stmt_tree_init(*buffer, ast, true);
	return (true);
}

void stmt_tree_destroy(struct statement *tree)
{
	free(tree);
}

static void inner(struct statement *stmt, size_t depth, vec_t *parent_vec, vec_t *cache, bool is_scope_change, size_t ret);

bool is_scope_modifier_kind(xre_expr_kind_t kind)
{
	switch (kind) {
	case __DO__:
	case __ELSE__:
	case __AND__:
	case __OR__:
	case __CLOSURE__:
	case __LOOP__:
		return (true);
	
	default:
		return (false);
	}
 }

static void put_binop(struct statement *stmt, size_t depth, vec_t *parent_vec, vec_t *cache, bool is_scope_change, size_t ret)
{
	inner(&__statements__[stmt->br.left], depth + 1, parent_vec, cache, is_scope_change, ret);
	inner(&__statements__[stmt->br.right], depth + 1, parent_vec, cache, is_scope_modifier_kind(stmt->orig->_kind), ret);
}

void put_sequence(struct statement *stmt, size_t depth, vec_t *parent_vec, vec_t *cache, bool is_scope_change, size_t ret)
{
	size_t c = vec_size(stmt->children);
	statement_name_t *s;
	 while (c--) {
	 	s = (statement_name_t *)vec_access(stmt->children, c);
	 	inner(&__statements__[*s],
	 	      depth + 1, parent_vec, cache, is_scope_change, ret);
	 }
}

static void put_uniop(struct statement *stmt, size_t depth, vec_t *parent_vec, vec_t *cache, bool is_scope_change, size_t ret)
{
	inner(&__statements__[stmt->br.left], depth + 1, parent_vec, cache, is_scope_change, ret);
}


static char* format_string(const char* format, ...) {
    va_list args;
    
    va_start(args, format);
    int len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    
    if (len < 0) {
        return NULL;
    }
    
    char* result = (char*)malloc(len + 1);
    if (!result) {
        return NULL;
    }
    

    va_start(args, format);
    vsnprintf(result, len + 1, format, args);
    va_end(args);
    
    return result; 
}

ssize_t is_cached(const char *str, vec_t *cache) {
	size_t size = vec_size(cache);
	while (size--) {
		const char *s = *(char **)vec_at(cache, size);
		if (!strcmp(s, str)) {
			return (size);
		}
	}
	return (-1);
}

static void inner(struct statement *stmt, size_t depth, vec_t *parent_vec, vec_t *cache, bool is_scope_change, size_t ret)
{
	vec_t *vec = parent_vec;

	if (is_scope_change) {
		ret += vec_size(vec);
		vec = stmt->local;
	}

	const char *str = "";
	if (stmt->orig->_type == EXPR_TYPE_VALUE) {
		switch (stmt->orig->_kind) {
		case __VARIABLE__:
			str = format_string("%s", stmt->string);
			break;
		
		case __VAL__:
			str = format_string("%d", stmt->value);
			break;
		
		case __STRING_LITERAL__:
			str = format_string("'%s'", stmt->string);
			break;
		
		case __SEQUENCE__:
			str = format_string("A");
			break;
		
		default:
			str = format_string("_");
			break;
		}

		ssize_t pos = is_cached(str, cache);
		size_t npos = 0;
		if (pos == -1) {
			npos = ret + vec_size(parent_vec);
		} else {
			npos = pos;
		}

		stmt->value = npos;

		if (pos == -1) {
			vec_push(vec, &str);
			vec_push(cache, &str);
		}
	}

	if (stmt->orig->_kind == __SEQUENCE_POINT__) {
		put_sequence(stmt, depth, vec, cache, false, ret);
	
	} else if (stmt->orig->_type & (EXPR_OP_TYPE_BINOP)) {
		put_binop(stmt, depth, vec, cache, false, ret);
	
	} else if (stmt->orig->_type & EXPR_OP_TYPE_UNIOP)
		put_uniop(stmt, depth, vec, cache, false, ret);
	
	if (is_scope_change) {
		size_t i = vec_size(stmt->local);
		while (i--) {
			vec_pop(cache, NULL);
		}
	}
	
}



static void inner2(struct statement *stmt, size_t depth, vec_t *cache, size_t vars);

static void put_binop2(struct statement *stmt, size_t depth, vec_t *cache, size_t vars)
{
	inner2(&__statements__[stmt->br.left], depth + 1, cache, vars);
	inner2(&__statements__[stmt->br.right], depth + 1, cache, vars);
}

void put_sequence2(struct statement *stmt, size_t depth, vec_t *cache, size_t vars)
{
	size_t c = vec_size(stmt->children);
	statement_name_t *s;
	 while (c--) {
	 	s = (statement_name_t *)vec_access(stmt->children, c);
	 	inner2(&__statements__[*s],
	 	      depth + 1, cache, vars);
	 }
}

static void put_uniop2(struct statement *stmt, size_t depth, vec_t *cache, size_t vars)
{
	inner2(&__statements__[stmt->br.left], depth + 1, cache,vars );
}

static void inner2(struct statement *stmt, size_t depth, vec_t *cache, size_t vars)
{
	size_t d = depth;
	while (d--) {
		write (1, "    ", 4);
	}

	if (stmt->orig->_type == EXPR_TYPE_VALUE) {
		printf("  &__var_%02zu__\n", stmt->value);
	} else {
		printf("[%s] %s (depth: %02zu, %zu:%zu)\n",  expr_type_to_string(stmt->orig->_type),  expr_kind_to_string(stmt->orig->_kind), stmt->orig->_depth, stmt->orig->_line, stmt->orig->_cols);
	}

	size_t size = vec_size(stmt->local);
	size_t i = 0;
	if (size) {
		while (i < size) {
			d = depth;
			while (d--) {
				write (1, "    ", 4);
			}
			printf("  PUSH __var_%02zu__ // %s\n", vars + i, *(char **)vec_access(stmt->local, i));
			i++;
		}
		vars += size;
	}

	if (stmt->orig->_kind == __SEQUENCE_POINT__) {
		put_sequence2(stmt, depth, cache, vars);
	
	} else if (stmt->orig->_type & (EXPR_OP_TYPE_BINOP)) {
		put_binop2(stmt, depth, cache, vars);
	
	} else if (stmt->orig->_type & EXPR_OP_TYPE_UNIOP) {
		put_uniop2(stmt, depth, cache, vars);
	}
	

	size = vec_size(stmt->local);
	if (size) {
		while (size--) {
			d = depth;
			while (d--) {
				write (1, "    ", 4);
			}
			printf("   POP __var_%02zu__\n", vars - size - 1);
		}
	}
}

void scoper(struct statement *tree) {
	vec_t *cache = vec_create(sizeof(char *), 64, NULL);
	inner(tree, 0, tree->local, cache, true, 0);
	inner2(tree, 0, cache, 0);
}

bool xre_runtime(xre_ast_t *ast)
{
	__return_val_if_fail__(ast, false);

	struct statement *self   = NULL;
	
	object_t    result = { 0 };

	if (!symtab_init() || !stmt_tree_create(ast, &self)) {
		goto out_of_memory;
	}

	if (!stack_init()) {
		stmt_tree_destroy(self);
		goto out_of_memory;
	}

	// if (!heap_init()) {
	// 	stmt_tree_destroy(self);
	// 	goto out_of_memory;
	// }

	__statements__ = self;

	scoper(self);
	if (!self->eval(self)) {
		return (xre_error(&_error),
			stmt_tree_destroy(self),
			false);
	}

	stack_pop(&result);

	if (__xre_args__.flags & SHOW_EXPR_RESULT) {
		result.repr(__object_get_data_as_any(&result));
		(void)fprintf(stderr, "\n");
	}

	return (object_drop(&result), stmt_tree_destroy(self), true);

out_of_memory:
	(void)fprintf(stderr, "Out of memory\n");
	return (false);
}
