#include "xre_assert.h"
#include "xre_log.h"
#include "xre_operations.h"
#include "xre_runtime.h"

bool simple_assignment(xre_frame_t *initial_frame, const char *key, xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);
  
  if (!symtab_set(initial_frame, key, frame->state)) {
    return (false);
  }

  return (state_copy_ref(initial_frame, frame));
}

bool reassignement(xre_frame_t *initial_frame, const char *key, xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = LEFT_CHILD(frame);
  if (!symtab_get(left->identifier)) {
    __return_error(frame, XRE_UNBOUND_LOCAL_ERROR);
  }

  if (!symtab_set(initial_frame, key, frame->state)) {
    return (false);
  }

  return (state_copy_ref(initial_frame, frame));
}

bool assignment_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = LEFT_CHILD(frame);
  xre_frame_t *right = RIGHT_CHILD(frame);

  if (left->kind != __IDENTIFIER__) {
    __return_error(frame, XRE_INVALID_ASSIGMENT_ERROR);
  }

  if (!evaluate(right)) {
    return (false);
  }

  if (frame->kind == __ASSIGN__) {
    return (simple_assignment(frame, left->identifier, right));
  }

  symtab_entry_t *item = symtab_get(left->identifier);
  if (!item) {
    __return_error(left, XRE_UNBOUND_LOCAL_ERROR);
  }

  xre_state_t *state = &item->state;

  if (!IS_FLAG_SET(*state, STATE_NUMBER)) {
    __return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
  }

  (void)state_value(left, state->value);
  
  switch (frame->kind) {
  case __ADD_ASSIGN__:
    if (!add_op(frame)) {
      return (false);
    }
    break;

  case __SUB_ASSIGN__:
    if (!sub_op(frame))
      return (false);
    break;

  case __MUL_ASSIGN__:
    if (!mul_op(frame)) {
      return (false);
    }  
    break;

  case __DIV_ASSIGN__:
    if (!div_op(frame)) {
      return (false);
    }
    break;

  case __MOD_ASSIGN__:
    if (!mod_op(frame)) {
      return (false);
    }
    break;

  case __POW_ASSIGN__:
  default:
    __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
  }

  return (reassignement(frame, left->identifier, frame));
}
