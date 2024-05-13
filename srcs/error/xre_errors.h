#ifndef __XRE_REPL_ERROR_H__
#define __XRE_REPL_ERROR_H__

#include "xre_parse.h"

typedef enum {
  XRE_ARITHMETIC_ERROR_C,
  XRE_INDEX_ERROR_C,
  XRE_NAME_ERROR_C,
  XRE_RUNTIME_ERROR_C,
  XRE_INTERNAL_ERROR_C,
  XRE_ACCESS_ERROR_C,
  XRE_MEMORY_ERROR_C,
  XRE_SYNTAX_ERROR_C,
  XRE_TERMINATION_ERROR_C,
  XRE_TYPE_ERROR_C,
  XRE_VALUE_ERROR_C,
} error_class_e;

typedef enum {
  XRE_ALREADY_ANNOTATED_ERROR,
  XRE_UNDEFINED_BEHAVIOR_ERROR,
  XRE_OUT_OF_MEMORY_ERROR,
  XRE_MAX_ITERATIONS_ERROR,
  XRE_MODE_WRITE_ON_STRICT_ERROR,
  XRE_WRITE_ON_READONLY_ERROR,
  XRE_INVALID_ANNOTATION_ERROR,
  XRE_UNKNOWN_ANNOTATION_ERROR,
  XRE_TYPE_MISSMATCH_ERROR,
  XRE_UNKNOWN_COMMAND_ERROR,
  XRE_NEGATIVE_SHIFT_ERROR,
  XRE_EXCEEDS_SHIFT_LIMIT_ERROR,
  XRE_INVALID_TYPE_FOR_OPERAND_ERROR,
  XRE_UNBOUND_LOCAL_ERROR,
  XRE_OUT_OF_BOUNDS_ACCESS_ERROR,
  XRE_NOT_IMPLEMENTED_ERROR,
  XRE_OVERFLOW_ERROR,
  XRE_UNDERFLOW_ERROR,
  XRE_ZERO_DIVISION_ERROR,
  XRE_INVALID_ASSIGMENT_ERROR,
  XRE_UNEXPECTED_OPERATOR_ERROR,
  XRE_UNEXPECTED_OPERAND_ERROR,
  XRE_UNMATCHED_PARENTHESIS_ERROR,
  XRE_UNTERMINATED_STRING_ERROR,
} error_type_e;

typedef struct {
  const xre_token_t *src;
  error_class_e class;
  error_type_e type;
} error_notification_t;

error_class_e error_type_to_class(error_type_e type);
const char *error_class_str(error_class_e class);
const char *error_type_str(error_type_e type);
void xre_error(error_notification_t *notification);

#define __return_error(frame, error_type) return (set_error(frame, error_type));
/* #define __return_error(frame, error_type) do { \
        XRE_LOGGER(debug, "trace"); \
        return (set_error(frame, error_type)); \
    } while (0) */
#endif /* __XRE_REPL_ERROR_H__ */
