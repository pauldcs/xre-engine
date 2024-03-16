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

typedef bool (*frame_function)(xre_frame_t *);

static frame_function dispatch_table[] = {
    [__ASSIGN__] = assignment_op,
    [__ADD_ASSIGN__] = assignment_op,
    [__SUB_ASSIGN__] = assignment_op,
    [__DIV_ASSIGN__] = assignment_op,
    [__MOD_ASSIGN__] = assignment_op,
    [__POW_ASSIGN__] = assignment_op,
    [__MUL_ASSIGN__] = assignment_op,
    [__ADD__] = arithmetic_op,
    [__SUB__] = arithmetic_op,
    [__MUL__] = arithmetic_op,
    [__DIV__] = arithmetic_op,
    [__MOD__] = arithmetic_op,
    [__POW__] = arithmetic_op,
    [__EQ__] = relational_op,
    [__NE__] = relational_op,
    [__LT__] = relational_op,
    [__GT__] = relational_op,
    [__LE__] = relational_op,
    [__GE__] = relational_op,
    [__BAND__] = bitwise_op,
    [__BOR__] = bitwise_op,
    [__BXOR__] = bitwise_op,
    [__LSHIFT__] = bitwise_op,
    [__RSHIFT__] = bitwise_op,
    [__LOOP__] = loop_op,
    [__SEQUENCE_POINT__] = sequence_op,
    [__SEPARATOR__] = separator_op,
    [__AT__] = at_op,
    [__DO__] = logical_op,
    [__ELSE__] = logical_op,
    [__AND__] = logical_op,
    [__OR__] = logical_op,
    [__PRINT__] = print_op,
    [__NOT__] = not_op,
    [__IDENTIFIER__] = identifier_op,
    [__VAL__] = value_op,
    [__STRING_LITERAL__] = string_op
};

bool evaluate(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);
    
    if (frame->kind < 0 || frame->kind >= sizeof(dispatch_table) / sizeof(dispatch_table[0])) {
        __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
    }

    frame_function func = dispatch_table[frame->kind];
    if (func) {
      return func(frame);
    }
  
    __return_error(frame, XRE_NOT_IMPLEMENTED_ERROR);
}

bool call_runtime(xre_ast_t *ast) {
  if (init_frame_tree(ast) < 0)
    return (false);
  
  bool ret = xre_runtime();
    
  deinit_frame_tree(&frame_tree_g[0]);
  free(frame_tree_g);
  frame_tree_g = NULL;
  
  return (ret);
}

bool xre_runtime(void) {
  xre_frame_t *frame = &frame_tree_g[0];

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
  
    XRE_LOGGER(error, "Failed unexpectedly");
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
