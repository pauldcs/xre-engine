#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_utils.h"
#include "xre_log.h"
#include <string.h>

bool operand(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, NULL);

  if (frame->kind == __IDENTIFIER__) {
    if (!strcmp(frame->initial.string, "exit")) {
      XRE_LOGGER(info, "exiting ...");
      return (set_error(frame, XRE_EXIT_ERROR, XRE_EXIT_CALLED_ERROR));
    }
  
    state_t *state = runtime_stack_get(frame->initial.string);

    if (state) {
      if (state->type == STATE_ARRAY) {
        array_t *array = array_pull(state->array, 0, -1);
        return (change_state_array(frame, array));
      }
      
      if (state->type == STATE_NUMBER) {
        return (change_state_value(frame, state->value));
      }

      if (state->type == STATE_STRING) {
        return (change_state_string(frame, strdup(state->string)));
      }

    
      XRE_LOGGER(warning, "Confusing condition");
      return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
    }
    
    return (set_error(frame, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));
  }
  
  switch (frame->kind) {
    case __VAL__:
      return change_state_value(frame, frame->initial.value);
    
    case __STRING_LITERAL__:
      return change_state_string(frame, strdup(frame->initial.string));
    default:
      break;
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
