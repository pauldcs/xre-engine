#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_assert.h"

bool sequence_op(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  array_t *array = NULL;

  if (!evaluate(left)
    || !evaluate(right)) {
    return (false);
  }

  if (left->event.type & STATE_ARRAY) {
    array = left->event.array;
  
  } else {
    array = array_create(sizeof(state_t), 10, NULL);
    array_push(array, &left->event);
  }
  
  array_push(array, &right->event);

  return (change_state_array(node, array));
}

bool separator_op(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (!evaluate(left) || !evaluate(right)) {
      return (false);
    }

  return (change_state_copy(node, right));
}
