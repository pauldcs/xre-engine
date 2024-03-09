#include "xre_runtime.h"
#include "array.h"
#include "xre_args.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_stringf.h"
#include "xre_utils.h"
#include <iso646.h>
#include <stdbool.h>
#include <sys/types.h>

bool        _has_error = false;
t_xre_error _error;

bool set_error(xre_runtime_t *frame, t_xre_error_type type, t_xre_error_subtype subtype) {
  _error.error.type = type;
  _error.error.subtype = subtype;
  _error.src = frame->token;
  _has_error = true;
  return (false);
}

bool error_occurred(void) {
  return (_has_error);
}

bool change_state_value(xre_runtime_t *frame, int64_t value) {
  frame->state.type = STATE_NUMBER;
  frame->state.value = value;
  return (true);
}

bool change_state_array(xre_runtime_t *frame, array_t *array) {
  frame->state.type = STATE_ARRAY;
  frame->state.array = array;
  return (true);
}

bool change_state_string(xre_runtime_t *frame, char *string) {
  frame->state.type = STATE_STRING;
  frame->state.string = string;
  return (true);
}

bool change_state_copy(xre_runtime_t *this, xre_runtime_t *that) {
  (void)memcpy(&this->state, &that->state, sizeof(state_t));
  return (true);
}

bool binop_exec(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->kind) {
  case __ASSIGN__:
  case __ADD_ASSIGN__:
  case __SUB_ASSIGN__:
  case __DIV_ASSIGN__:
  case __MOD_ASSIGN__:
  case __POW_ASSIGN__:
  case __MUL_ASSIGN__:
  case __LSHIFT_ASSIGN__:
  case __RSHIFT_ASSIGN__:
  case __OR_ASSIGN__:
  case __AND_ASSIGN__:
    return (assignment_op(frame));

  case __ADD__:
  case __SUB__:
  case __MUL__:
  case __DIV__:
  case __MOD__:
  case __POW__:
    return (arithmetic_op(frame));

  case __EQ__:
  case __NE__:
  case __LT__:
  case __GT__:
  case __LE__:
  case __GE__:
    return (relational_op(frame));

  case __BAND__:
  case __BOR__:
  case __BXOR__:
  case __LSHIFT__:
  case __RSHIFT__:
    return (bitwise_op(frame));
  
  case __LOOP__:
    return (loop_op(frame));

  case __SEQUENCE_POINT__:
    return (sequence_op(frame));
  
  case __SEPARATOR__:
    return (separator_op(frame));
  
  case __INJECT__:
    return (inject_op(frame));
  
  case __DO__:
  case __ELSE__:
  case __AND__:
  case __OR__:
    return (logical_op(frame));
  
  default:
    break;
  }

  return (set_error(frame, XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR));
}

bool uniop_exec(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->kind) {
  case __NOT__:
    return (not_op(frame));
    
  default:
    return (set_error(frame, XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR));
  }
}

bool operand_exec(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, NULL);

  return (operand(frame));
}


bool evaluate(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->type) {
  case EXPR_TYPE_VALUE:
    return operand_exec(frame);

  case EXPR_OP_TYPE_BINOP:
    return binop_exec(frame);

  case EXPR_OP_TYPE_UNIOP:
    return uniop_exec(frame);

  case EXPR_TYPE_OTHER:
  default:
    XRE_LOGGER(warning, "Confusing condition");
    return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
  }
}

bool xre_runtime(xre_ast_t *ast) {
  __return_val_if_fail__(ast, false);

  xre_runtime_t *frame = init_state(ast);

  if (!runtime_stack_init()) {
    return (false);
  }
  
  if (!evaluate(frame)) {
    if (error_occurred()) { 
      xre_error(&_error, _error.src);
      return (deinit_state(frame), false);
    }
    XRE_LOGGER(error, "Failed without error message");
    return (deinit_state(frame), false);
  }
  
  if (__xre_args__.flags & SHOW_EXPR_RESULT) {
    state_print(frame);
  }
  
  runtime_stack_deinit();
  deinit_state(frame);

  return (true);
}
