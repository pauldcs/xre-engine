#include "xre_assert.h"
#include "xre_runtime.h"

frame_block_t *loop_op(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  size_t max_iterations = DEFAULT_MAX_ITERATIONS;
  frame_block_t *condition = NULL;
  frame_block_t *consequence = NULL;

  for (;;) {
    if (!max_iterations--) {
      return (error_block_alloc(XRE_RUNTIME_ERROR, XRE_NOT_TERMINATED_ERROR));
    }

    condition = evaluate(node->binop.left);
    if (condition->_error != NULL) {
      return (condition);
    }

    if (!is_truthy_block(condition)) {
      break;
    }

    frame_block_free(&condition);

    consequence = evaluate(node->binop.right);
    if (consequence->_error != NULL) {
      return (consequence);
    }

    frame_block_free(&consequence);
  }

  frame_block_free(&condition);
  return (true_block_alloc());
}
