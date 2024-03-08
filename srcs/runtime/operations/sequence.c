#include "xre_runtime.h"
#include "xre_frame.h"
#include "xre_assert.h"

frame_block_t *sequence_op(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  array_t *array = NULL;

  frame_block_t *left = evaluate(node->binop.left);
  if (left->_error != NULL) {
    return (left);
  }

  frame_block_t *right = evaluate(node->binop.right);
  if (right->_error != NULL) {
    frame_block_free(&left);
    return (right);
  }

  if (left->_type & IF_SEQUENCE) {
    array = left->_data.array;
    free(left);
  
  } else {
    array = array_create(sizeof(frame_block_t), 10, NULL);
    array_push(array, left);
    frame_block_free(&left);
  
    n_arrays_allocd++;
  }
  
  array_push(array, right);
  frame_block_free(&right);

  return (sequence_block_alloc(array));
}

frame_block_t *separator_op(t_xre_ast *node) {
  __return_val_if_fail__(node, NULL);

  frame_block_t *left = evaluate(node->binop.left);
  if (left->_error != NULL) {
    return (left);
  }

  frame_block_t *right = evaluate(node->binop.right);
  if (right->_error != NULL) {
    frame_block_free(&left);
    return (right);
  }

  frame_block_free(&left);

  return (is_truthy_block(right) ? true_block_with(right) : false_block_with(right));
}
