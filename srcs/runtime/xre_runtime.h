#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_log.h"
#include "xre_errors.h"
#include "xre_parse.h"
#include <stdbool.h>

#define DEFAULT_MAX_ITERATIONS 5000
#define DEFAULT_FRAME_SIZE 64

typedef enum {
  STATE_ARRAY,
  STATE_NUMBER,
  STATE_STRING,
  STATE_UNDEFINED,
} state_e;

typedef struct {
  state_e type;

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
  xre_token_t     *token;
  xre_frame_t     *left;
  xre_frame_t     *right;
  const char      *identifier;
  bool            is_ref;
  state_t         state;

} xre_frame_t;

xre_frame_t *state_init(xre_ast_t *ast);
void state_deinit(xre_frame_t *frame);
void state_free(state_t *state);
void state_print(xre_frame_t *frame);
bool is_true_state(xre_frame_t *frame);
const char *state_to_str(state_t *state);
void state_debug (xre_frame_t *frame);

extern array_t *symtab;

typedef struct {
  const char *key;
  state_t state;
} symtab_entry_t;

bool symtab_init(void);
void symtab_deinit(void);

symtab_entry_t *symtab_get(const char *key);
bool            symtab_set(xre_frame_t *frame, const char *key, state_t state);

extern bool                 _has_error;
extern error_notification_t _error;

bool set_error(xre_frame_t *node, error_type_e type);
bool error_occurred(void);

bool state_value(xre_frame_t *frame, int64_t value);
bool state_array_ref(xre_frame_t *frame, array_t *array);
bool state_array(xre_frame_t *frame, array_t *array);
bool state_string_ref(xre_frame_t *frame, char *string);
bool state_copy_ref(xre_frame_t *this, xre_frame_t *that);

bool call_runtime(xre_ast_t *ast);
bool xre_runtime(xre_frame_t *frame);
bool evaluate(xre_frame_t *frame);

#endif /* __XRE_RUNTIME_H__ */
