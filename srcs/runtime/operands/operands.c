#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_utils.h"
#include "xre_log.h"

bool operand(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  if (node->kind == __IDENTIFIER__) {
    if (!strcmp(node->string, "exit")) {
      XRE_LOGGER(info, "exiting ...");
      return (set_error(node, XRE_EXIT_ERROR, XRE_EXIT_CALLED_ERROR));
    }
  
    state_t *state = runtime_stack_get(node->string);

    if (state) {
      if (state->type == STATE_ARRAY) {
        return (change_state_array(node, state->array));
      }
      
      if (state->type == STATE_NUMBER) {
        return (change_state_value(node, state->value));
      }

      if (state->type == STATE_STRING) {
        return (change_state_string(node, state->string));
      }

    
      XRE_LOGGER(warning, "Confusing condition");
      return (set_error(node, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
    }
    
    return (set_error(node, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));
  }
  
  switch (node->kind) {
    case __VAL__:
      return change_state_value(node, node->value);
    
    case __STRING_LITERAL__:
      return change_state_string(node, node->string);
    default:
      break;
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (set_error(node, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
