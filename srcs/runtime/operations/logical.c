#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_parse.h"
#include <stdbool.h>

bool do_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (!evaluate(left)) {
    return (false);
  }

  if (is_truthy_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
    return (change_state_copy(node, right));
  }

  return (change_state_copy(node, left));
}

bool else_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (!evaluate(left)) {
    return (false);
  }

  if (!is_truthy_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
    return (change_state_copy(node, right));
  }

  return (change_state_copy(node, left));
}

bool and_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (!evaluate(left)) {
    return (false);
  }

  if (is_truthy_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
  }

  return (change_state_value(node, is_truthy_state(right)));
}

bool or_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (!evaluate(left))
    return (false);

  if (is_truthy_state(left)) {
    return (change_state_value(node, true));
  }
  
  if (!evaluate(right)) {
    return (false);
  }

  return (change_state_value(node, is_truthy_state(right)));
}

bool logical_op(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  switch (node->kind) {
  case __DO__:
    return (do_op(node));
  
  case __AND__:
    return (and_op(node));
  
  case __ELSE__:
    return (else_op(node));
  
  case __OR__:
    return (or_op(node));
  
  default:
    break;
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (set_error(node, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
