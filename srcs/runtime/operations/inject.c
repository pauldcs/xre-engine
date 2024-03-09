#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_parse.h"

bool inject_op(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (left->kind != __IDENTIFIER__) {
    return set_error(node, XRE_TYPE_ERROR, XRE_INVALID_ASSIGMENT_ERROR);
  }

  if (!evaluate(right)) {
    return (false);
  }

  state_print(right);

  bool is_truthy = is_truthy_state(right);

  return (change_state_value(node, is_truthy));
}
