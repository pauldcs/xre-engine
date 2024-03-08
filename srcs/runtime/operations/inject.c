#include "xre_runtime.h"
#include "xre_assert.h"

frame_block_t *inject_op(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  if (node->binop.left->kind != __IDENTIFIER__) {
    return error_block_alloc(XRE_TYPE_ERROR, XRE_INVALID_ASSIGMENT_ERROR);
  }

  frame_block_t *right = evaluate(node->binop.right);
  if (right->_error != NULL) {
    return (right);
  }

  frame_block_print(right);

  bool is_truthy = is_truthy_block(right);


  return (is_truthy ? true_block_with(right) : false_block_with(right));
}
