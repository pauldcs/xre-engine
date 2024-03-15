#include "xre_assert.h"
#include "xre_log.h"
#include "xre_operations.h"
#include "xre_runtime.h"
#include <stdbool.h>

bool at_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left)) {
    
    return (false);
  }

  if (!evaluate(right)) {

    return (false);
  }

  if (left->state.type != STATE_ARRAY) {
  
    __return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
  }

  if (right->state.type != STATE_NUMBER) {
  
    __return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
  }

  array_t *array = left->state.array;
  size_t value = right->state.value;
  
  if (value >= array_size(array)) {

    __return_error(frame,  XRE_OUT_OF_BOUNDS_ACCESS_ERROR);
  }
  
  xre_frame_t *f = (xre_frame_t *)array_at(array, value);
  switch (f->state.type) {
    case STATE_STRING:
      return (change_state_string(frame, f->state.string));
    case STATE_ARRAY:
      return (change_state_array(frame, f->state.array));
    case STATE_NUMBER:
      return (change_state_value(frame, f->state.value));
    case STATE_UNDEFINED:
      break;
  }
  
  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}