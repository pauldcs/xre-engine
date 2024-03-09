#ifndef __XRE_RUNTIME_H__
# define __XRE_RUNTIME_H__

# include "xre_parse.h"
# include "xre_errors.h"
# include "array.h"
# include <sys/types.h>
# include <limits.h>
# include <stdbool.h>

# define DEFAULT_MAX_ITERATIONS 500
# define DEFAULT_FRAME_SIZE 64

# define IF_BUILTIN  1ULL << 1
# define IF_INTEGER  1ULL << 2
# define IF_STRING   1ULL << 3
# define IF_ARRAY    1ULL << 4
# define IF_BOOL     1ULL << 5
# define IF_INJECT   1ULL << 7

#define IS_ADDABBLE(a, b, max) (a <= max - b)
#define IS_MULABBLE(a, b, max) (b == 0 || a <= max / b)
#define IS_SUBABBLE(a, b, min) (a >= min + b)

#define I64_IS_MULABBLE(a, b) IS_MULABBLE(a, b, LLONG_MAX)
#define I64_IS_ADDABBLE(a, b) IS_ADDABBLE(a, b, LLONG_MAX)
#define I64_IS_SUBABBLE(a, b) IS_SUBABBLE(a, b, 0)

extern bool        _has_error;
extern t_xre_error _error;
bool set_error(xre_ast_t*node, t_xre_error_type type, t_xre_error_subtype subtype);
bool error_occurred(void);


bool xre_runtime(xre_ast_t *ast);
bool evaluate(xre_ast_t *ast);

bool arithmetic_op(xre_ast_t *node);
bool relational_op(xre_ast_t *node);
bool bitwise_op(xre_ast_t *node);
bool loop_op(xre_ast_t *node);
bool logical_op(xre_ast_t *node);
bool assignment_op(xre_ast_t *node);
bool not_op(xre_ast_t *node);
bool sequence_op(xre_ast_t *node);
bool inject_op(xre_ast_t *node);
bool separator_op(xre_ast_t *node);

bool add_op(xre_ast_t *node);
bool sub_op(xre_ast_t *node);
bool mul_op(xre_ast_t *node);
bool div_op(xre_ast_t *node);
bool mod_op(xre_ast_t *node);

bool operand(xre_ast_t *ast);

bool change_state_value(xre_ast_t *node, int64_t value);
bool change_state_array(xre_ast_t *node, array_t *array);
bool change_state_string(xre_ast_t *node, const char *string);
bool change_state_copy(xre_ast_t *this, xre_ast_t *that);

#endif /* __XRE_RUNTIME_H__ */
