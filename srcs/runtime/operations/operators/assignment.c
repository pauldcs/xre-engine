#include "xre_assert.h"
#include "xre_frame.h"
#include "xre_log.h"
#include "xre_operations.h"
#include "xre_runtime.h"

bool simple_assignment(xre_frame_t *initial_frame, const char *key, varmode_e mode, xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);
  
  if (!runtime_variables_set(
      initial_frame,
      key,
      frame->state,
      mode
    )) {
    return (false);
  }
  
  change_state_copy(initial_frame, frame);
  return (true);
}

bool reassignement(xre_frame_t *initial_frame, const char *key, varmode_e mode, xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  if (!runtime_variables_get(left->initial.string)) {
    
    log_error_condition_reached;
    return (set_error(left, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));
  }

  if (!runtime_variables_set(
      initial_frame,
      key,
      frame->state,
      mode
    )) {
    return (false);
  }

  change_state_copy(initial_frame, frame);
  return (true);
}

bool assignment_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (left->kind == __ANNOTATE__
    && !evaluate(left)) {

    log_error_return;
    return (false);
  }

  if (left->kind != __IDENTIFIER__ ) {
    log_error_condition_reached;
    return set_error(frame, XRE_TYPE_ERROR, XRE_INVALID_ASSIGMENT_ERROR);
  }

  if (!evaluate(right)) {

    log_error_return;
    return (false);
  }

  if (frame->kind == __ASSIGN__) {
    return (simple_assignment(frame, left->initial.string, left->mode, right));
  }

  stack_item_t *item = runtime_variables_get(left->initial.string);
  if (!item) {
    log_error_condition_reached;
    return set_error(right, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR);
  }

  state_t *state = &item->state;

  if (state->type != STATE_NUMBER) {
    log_error_condition_reached;
    return set_error(right, XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND);
  }

  change_state_value(left, state->value);
  
  switch (frame->kind) {
  case __ADD_ASSIGN__:
    if (!add_op(frame))
      return (false);
    break;

  case __SUB_ASSIGN__:
    if (!sub_op(frame))
      return (false);
    break;

  case __MUL_ASSIGN__:
    if (!mul_op(frame))
      return (false);
    break;

  case __DIV_ASSIGN__:
    if (!div_op(frame))
      return (false);
    break;

  case __MOD_ASSIGN__:
    if (!mod_op(frame))
      return (false);
    break;

  case __POW_ASSIGN__:
  default:
    log_error_condition_reached;
    return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
  }

  return (reassignement(frame, left->initial.string, left->mode, frame));
}
