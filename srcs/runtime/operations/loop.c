#include "xre_assert.h"
#include "xre_runtime.h"
#include "xre_parse.h"

bool loop_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  size_t max_iterations = DEFAULT_MAX_ITERATIONS;

  for (;;) {
    if (!max_iterations--) {
      return (set_error(node, XRE_RUNTIME_ERROR, XRE_NOT_TERMINATED_ERROR));
    }

    if (!evaluate(left)) {
      return (false);
    }

    if (!is_truthy_state(left)) {
      break;
    }
    //printf("bite1\n");

    if (!evaluate(right)) {
      return (false);
    }
  
    (void)change_state_copy(node, right);
  }

  return (change_state_copy(node, left));
}
