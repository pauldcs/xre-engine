#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_args.h"
#include "xre_assert.h"
#include "xre_log.h"
#include "xre_operations.h"
#include <stdbool.h>

bool _has_error = false;
error_notification_t _error;

bool set_error(xre_frame_t *frame, error_type_e type) {
  _error.class = error_type_to_class(type);
  _error.type = type;
  _error.src = frame->token;
  _has_error = true;
  return (false);
}

bool error_occurred(void) { return (_has_error); }

bool binop_exec(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->kind) {
  case __ASSIGN__:
  case __ADD_ASSIGN__:
  case __SUB_ASSIGN__:
  case __DIV_ASSIGN__:
  case __MOD_ASSIGN__:
  case __POW_ASSIGN__:
  case __MUL_ASSIGN__:
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
  
  case __AT__:
    return (at_op(frame));

  case __DO__:
  case __ELSE__:
  case __AND__:
  case __OR__:
    return (logical_op(frame));

  default:
    break;
  }

  __return_error(frame, XRE_NOT_IMPLEMENTED_ERROR);
}

bool uniop_exec(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->kind) {
  case __PRINT__:
    return (print_op(frame));
  
  case __NOT__:
    return (not_op(frame));

  default:
    __return_error(frame, XRE_NOT_IMPLEMENTED_ERROR);
  }
}

bool operand_exec(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->kind) {
    case __IDENTIFIER__:
      return (identifier_operand(frame));
    
    case __VAL__: 
    case __STRING_LITERAL__:
      return (basic_operand(frame));
  
  default:
    __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
  }
}

bool evaluate(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (expr_type_by_kind(frame->kind)) {
  case EXPR_TYPE_VALUE:
    return operand_exec(frame);

  case EXPR_OP_TYPE_BINOP:
    return binop_exec(frame);

  case EXPR_OP_TYPE_UNIOP:
    return uniop_exec(frame);

  case EXPR_TYPE_OTHER:
  default:
    __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
  }
}

bool call_runtime(xre_ast_t *ast) {
  xre_frame_t *frame = state_init(ast);
  if (frame) {
    bool ret = xre_runtime(frame);
    state_deinit(frame);
    return (ret);
  }

  (void)ast;
  return (false);
}

bool xre_runtime(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);


  if (!symtab_init()) {
    return (false);
  }

  if (!evaluate(frame)) {
    if (error_occurred()) {
      xre_error(&_error);
      if (_error.type == XRE_EXIT_CALLED_ERROR)
        exit(1);
      
      goto prison;
    }
  
    XRE_LOGGER(error, "Failed without error message");
    goto prison;
  }

  if (__xre_args__.flags & SHOW_EXPR_RESULT) {
    state_print(frame);
  }

  //state_debug(frame);
  symtab_deinit();
  return (true);

prison:
  symtab_deinit();
  return (false);
}
