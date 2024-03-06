#include "xre_runtime.h"
#include "array.h"
#include "xre_args.h"
#include "xre_assert.h"
#include "xre_frame.h"
#include "xre_errors.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_stringf.h"
#include "xre_utils.h"
#include <iso646.h>
#include <stdbool.h>
#include <sys/types.h>

static frame_block_t *return_frame_block(t_xre_ast *src, frame_block_t *block) {
  if (!block) {
    XRE_LOGGER(warning, "Confusing condition");
    return (error_block_alloc(XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
  }

  if (!block->_src)
    block->_src = src;

  return (block);
}

static frame_block_t *binop_exec(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  frame_block_t *left = NULL;
  frame_block_t *right = NULL;
  t_xre_expr_kind kind = node->kind;

  switch (kind) {
  case __ASSIGN__:
  case __ADD_ASSIGN__:
  case __SUB_ASSIGN__:
  case __DIV_ASSIGN__:
  case __MOD_ASSIGN__:
  case __POW_ASSIGN__:
  case __MUL_ASSIGN__:
  case __LSHIFT_ASSIGN__:
  case __RSHIFT_ASSIGN__:
  case __OR_ASSIGN__:
  case __AND_ASSIGN__:
    return (
        return_frame_block(node, assignment_op(node->kind, node->binop.left,
                                               evaluate(node->binop.right))));

  case __ADD__:
  case __SUB__:
  case __MUL__:
  case __DIV__:
  case __MOD__:
  case __POW__:
    return (return_frame_block(
        node, arithmetic_op(node->kind, evaluate(node->binop.left),
                            evaluate(node->binop.right))));

  case __EQ__:
  case __NE__:
  case __LT__:
  case __GT__:
  case __LE__:
  case __GE__:
    return (return_frame_block(
        node, relational_op(node->kind, evaluate(node->binop.left),
                            evaluate(node->binop.right))));

  case __BAND__:
  case __BOR__:
  case __BXOR__:
  case __LSHIFT__:
  case __RSHIFT__:
    return (return_frame_block(node, bitwise_op(node->kind,
                                                evaluate(node->binop.left),
                                                evaluate(node->binop.right))));

  case __SEQUENCE_POINT__:
  case __INJECT__:
  case __ANNOTATE__:
  case __SCOPE_RESOLUTION__:

  default:
    break;
  }

  frame_block_free(&left);
  frame_block_free(&right);

  return (return_frame_block(
      node, error_block_alloc(XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR)));
}

static frame_block_t *uniop_exec(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  switch (node->kind) {
  case __NOT__:
    return (return_frame_block(node, not_op(evaluate(node->binop.left))));
    
  default:
    return (return_frame_block(
        node, error_block_alloc(XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR)));
  }
}

static frame_block_t *logical_exec(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  return (return_frame_block(node, logical_op(node)));
}

static frame_block_t *operand_exec(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  return (return_frame_block(node, operand(node)));
}

static frame_block_t *loop_exec(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  return (return_frame_block(node, loop_op(node)));
}

static frame_block_t *sequence_exec(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  return (return_frame_block(node, sequence_op(node)));
}

static frame_block_t *inject_exec(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  return (return_frame_block(node, inject_op(node)));
}

frame_block_t *evaluate(t_xre_ast *ast) {
  __return_val_if_fail__(ast, NULL);

  switch (ast->type) {
  case EXPR_TYPE_OPERAND:
    return operand_exec(ast);

  case EXPR_TYPE_CONDITION:
    return logical_exec(ast);

  case EXPR_TYPE_LOOP:
    return loop_exec(ast);

  case EXPR_TYPE_SEQUENCE:
    return sequence_exec(ast);

  case EXPR_TYPE_INJECT:
    return inject_exec(ast);

  case EXPR_OP_TYPE_BINOP:
    return binop_exec(ast);

  case EXPR_OP_TYPE_UNIOP:
    return uniop_exec(ast);

  case EXPR_TYPE_OTHER:
  default:
    XRE_LOGGER(warning, "Confusing condition");
    return (return_frame_block(
        ast, error_block_alloc(XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION)));
  }
}

bool xre_runtime(t_xre_ast *ast) {
  __return_val_if_fail__(ast, false);

  if (!runtime_stack_init()) {
    return (false);
  }

  frame_block_t *result = evaluate(ast);
  if (!result) {
    XRE_LOGGER(error, "NULL result");
    return (false);
  }
  if (result->_error != NULL) {
    if (result->_error->error.type == XRE_EXIT_ERROR) {
      frame_block_free(&result);
      runtime_stack_deinit();
      return (false);
    }
    frame_block_print(result);
  
  } else if (__xre_args__.flags & SHOW_EXPR_RESULT) {
    frame_block_print(result);
  }

  if (result->_type & IF_SEQUENCE)
    array_kill(result->_data.array);
  
  frame_block_free(&result);
  runtime_stack_deinit();

  if (__xre_args__.flags & SHOW_STATISTICS) {
    print_statistics();
  }

  return (true);
}
