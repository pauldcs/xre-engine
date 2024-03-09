#ifndef __XRE_FRAME_H__
#define __XRE_FRAME_H__

#include "xre_errors.h"

typedef enum {
  STATE_ARRAY,
  STATE_NUMBER,
  STATE_STRING,
  STATE_UNDEFINED,
} exp_event_e;

typedef struct {
  exp_event_e type;
  union {
    int64_t value;
    array_t *array;
    char *string;
  };
} state_t;

typedef struct xre_frame_s xre_frame_t;

typedef struct xre_frame_s {
  xre_expr_kind_t kind;
  xre_expr_type_t type;
  xre_token_t *token;
  xre_frame_t *left;
  xre_frame_t *right;
  union {
    const char *string;
    int64_t value;
  } initial;

  state_t state;
} xre_frame_t;

typedef struct {
  const char *key;
  state_t state;
} stack_item_t;

xre_frame_t *state_init(xre_ast_t *ast);
void state_deinit(xre_frame_t *frame);
void state_free(state_t *state);
void state_print(xre_frame_t *frame);
bool is_true_state(xre_frame_t *frame);

extern array_t *runtime_variables;

// FRAME
bool runtime_variables_init(void);
void runtime_variables_deinit(void);

state_t *runtime_variables_get(const char *key);
bool runtime_variables_add(const char *key, state_t state);
bool runtime_variables_set(const char *key, state_t state);

extern bool _has_error;
extern t_xre_error _error;
bool set_error(xre_frame_t *node, t_xre_error_type type,
               t_xre_error_subtype subtype);
bool error_occurred(void);

bool change_state_value(xre_frame_t *frame, int64_t value);
bool change_state_array(xre_frame_t *frame, array_t *array);
bool change_state_string(xre_frame_t *frame, char *string);
bool change_state_copy(xre_frame_t *this, xre_frame_t *that);

#endif /* __XRE_FRAME_H__ */
