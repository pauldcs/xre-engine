#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_runtime.h"

void push_frame(array_t *array, xre_frame_t *frame) {
  if (frame->state.type == STATE_ARRAY) {
    array_t *array_2 = frame->state.array;
    size_t size = array_size(array_2);
    size_t i = 0;
    while (i < size) {
      xre_frame_t *frame = (xre_frame_t *)array_at(array_2, i);
      array_push(array, frame);
      i++;
    }
   } else {
    array_push(array, frame);
  }
}

bool sequence_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left) || !evaluate(right)) {
    return (false);
  }

  array_t *array = NULL;

  if (frame->state.type == STATE_ARRAY)
    array = frame->state.array;
  else
    array = array_create(sizeof(xre_frame_t), 8, NULL);

  push_frame(array, left);
  push_frame(array, right);

  frame->state.type = STATE_ARRAY;
  frame->state.array = array;


  return (true);
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
