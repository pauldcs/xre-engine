#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_runtime.h"

bool sequence_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  array_t *array = NULL;

  if (!evaluate(left) || !evaluate(right)) {
    return (false);
  }

  if (left->state.type == STATE_ARRAY) {
    array = left->state.array;

  } else {
    array = array_create(sizeof(state_t), 10, NULL);
    array_push(array, &left->state);
  }

  array_push(array, &right->state);

  return (change_state_array(frame, array));
}

bool separator_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left) || !evaluate(right)) {
    return (false);
  }

  return (change_state_copy(frame, right));
}
