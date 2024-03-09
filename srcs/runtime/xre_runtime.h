#ifndef __XRE_RUNTIME_H__
# define __XRE_RUNTIME_H__

# include "xre_parse.h"
# include "xre_errors.h"
# include "array.h"
# include <sys/types.h>
# include <limits.h>
# include <stdbool.h>

# define DEFAULT_MAX_ITERATIONS 5000
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

typedef enum {
	STATE_ARRAY,
	STATE_NUMBER,
	STATE_STRING,
  STATE_UNDEFINED,
}	exp_event_e;

typedef struct {
  exp_event_e  type;
  union {
    int64_t    value;
    array_t    *array;
    char *string;
};
} state_t;

typedef struct {
  const char  *key;
  state_t     state;
} stack_item_t;

typedef struct xre_runtime_s xre_runtime_t;

typedef struct xre_runtime_s {
  xre_expr_kind_t kind;
  xre_expr_type_t type;
  xre_token_t     *token;
  union {
    const char *string;
    int64_t value;
  } initial;
  
  state_t state;

  xre_runtime_t *left;
  xre_runtime_t *right;
} xre_runtime_t;

xre_runtime_t *init_state(xre_ast_t *ast);
void           deinit_state(xre_runtime_t *frame);
void           free_state(state_t *state);

extern array_t *runtime_stack;

// FRAME
bool    runtime_stack_init(void);
void    runtime_stack_deinit(void);


state_t *runtime_stack_get(const char *key);
bool     runtime_stack_add(const char *key, state_t state);
bool     runtime_stack_set(const char *key, state_t state);


// COMMON
void state_print(xre_runtime_t *frame);
bool is_truthy_state(xre_runtime_t *frame);

extern bool        _has_error;
extern t_xre_error _error;
bool               set_error(xre_runtime_t*node, t_xre_error_type type, t_xre_error_subtype subtype);
bool               error_occurred(void);

bool xre_runtime(xre_ast_t *ast);
bool evaluate(xre_runtime_t *frame);

bool arithmetic_op(xre_runtime_t *frame);
bool relational_op(xre_runtime_t *frame);
bool bitwise_op(xre_runtime_t *frame);
bool loop_op(xre_runtime_t *frame);
bool logical_op(xre_runtime_t *frame);
bool assignment_op(xre_runtime_t *frame);
bool not_op(xre_runtime_t *frame);
bool sequence_op(xre_runtime_t *frame);
bool inject_op(xre_runtime_t *frame);
bool separator_op(xre_runtime_t *frame);

bool add_op(xre_runtime_t *frame);
bool sub_op(xre_runtime_t *frame);
bool mul_op(xre_runtime_t *frame);
bool div_op(xre_runtime_t *frame);
bool mod_op(xre_runtime_t *frame);

bool operand(xre_runtime_t *frame);

bool change_state_value(xre_runtime_t *frame, int64_t value);
bool change_state_array(xre_runtime_t *frame, array_t *array);
bool change_state_string(xre_runtime_t *frame, char *string);
bool change_state_copy(xre_runtime_t *this, xre_runtime_t *that);

#endif /* __XRE_RUNTIME_H__ */
