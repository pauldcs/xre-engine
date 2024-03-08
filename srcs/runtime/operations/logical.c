#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include <stdbool.h>

frame_block_t *do_op(frame_block_t *condition, t_xre_ast *consequence) {
  if (condition->_error != NULL)
    return ((frame_block_t *)condition);

  if (is_truthy_block(condition)) {
    frame_block_free(&condition);
    return (evaluate(consequence));
  }

  return (condition);
}

frame_block_t *else_op(frame_block_t *condition, t_xre_ast *consequence) {
  if (condition->_error != NULL)
    return ((frame_block_t *)condition);

  if (!is_truthy_block(condition)) {
    frame_block_free(&condition);
    return (evaluate(consequence));
  }

  return ((frame_block_t *)condition);
}

frame_block_t *and_op(frame_block_t *condition, t_xre_ast *consequence) {
  frame_block_t *block = NULL;

  if (condition->_error != NULL)
    return (condition);

  if (is_truthy_block(condition)) {
    frame_block_free(&condition);

    block = evaluate(consequence);
    if (block->_error != NULL)
      return (block);

    if (is_truthy_block(block)) {
      return (true_block_with(block));
    }

    return (false_block_with(block));
  }

  return (false_block_with(condition));
}

frame_block_t *or_op(frame_block_t *condition, t_xre_ast *consequence) {
  frame_block_t *block = NULL;

  if (condition->_error != NULL)
    return (condition);

  if (!is_truthy_block(condition)) {
    frame_block_free(&condition);

    block = evaluate(consequence);
    if (block->_error != NULL)
      return (block);

    if (is_truthy_block(block)) {
      return (true_block_with(block));
    }

    return (false_block_with(block));
  }

  return (true_block_with(condition));
}

frame_block_t *logical_op(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  frame_block_t *condition = evaluate(node->binop.left);
  if (condition->_error != NULL)
    return (condition);

  switch (node->kind) {
  case __DO__:
    return (do_op(condition, node->binop.right));
  case __AND__:
    return (and_op(condition, node->binop.right));
  case __ELSE__:
    return (else_op(condition, node->binop.right));
  case __OR__:
    return (or_op(condition, node->binop.right));
  default:
    break;
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (error_block_alloc(XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
