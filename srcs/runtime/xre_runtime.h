#ifndef __XRE_RUNTIME_H__
# define __XRE_RUNTIME_H__

# include "xre_frame.h"
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
# define IF_SEQUENCE 1ULL << 6
# define IF_INJECT   1ULL << 7

#define IS_ADDABBLE(a, b, max) (a <= max - b)
#define IS_MULABBLE(a, b, max) (b == 0 || a <= max / b)
#define IS_SUBABBLE(a, b, min) (a >= min + b)

#define I64_IS_MULABBLE(a, b) IS_MULABBLE(a, b, LLONG_MAX)
#define I64_IS_ADDABBLE(a, b) IS_ADDABBLE(a, b, LLONG_MAX)
#define I64_IS_SUBABBLE(a, b) IS_SUBABBLE(a, b, 0)

bool           xre_runtime(t_xre_ast *ast);
frame_block_t *evaluate(t_xre_ast *ast);

frame_block_t* arithmetic_op(t_xre_expr_kind kind, frame_block_t* lv, frame_block_t* rv);
frame_block_t* relational_op(t_xre_expr_kind kind, frame_block_t* lv, frame_block_t* rv);
frame_block_t* bitwise_op(t_xre_expr_kind kind, frame_block_t* lv, frame_block_t* rv);
frame_block_t *loop_op(t_xre_ast *node);
frame_block_t *logical_op(t_xre_ast *node);
frame_block_t *assignment_op(t_xre_expr_kind kind, t_xre_ast *node, frame_block_t *rv);
frame_block_t* not_op(frame_block_t *block);
frame_block_t *sequence_op(t_xre_ast *node);
frame_block_t *inject_op(t_xre_ast *node);

frame_block_t *add_op(frame_block_t *lv, frame_block_t *rv);
frame_block_t *sub_op(frame_block_t *lv, frame_block_t *rv);
frame_block_t *mul_op(frame_block_t *lv, frame_block_t *rv);
frame_block_t *div_op(frame_block_t *lv, frame_block_t *rv);
frame_block_t *mod_op(frame_block_t *lv, frame_block_t *rv);

frame_block_t* operand(t_xre_ast *ast);

#endif /* __XRE_RUNTIME_H__ */
