#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_errors.h"
#include "xre_log.h"
#include "xre_parse.h"
#include <stdbool.h>

#define DEFAULT_MAX_ITERATIONS 5000
#define DEFAULT_FRAME_SIZE 64

typedef enum {
  STATE_ARRAY,
  STATE_NUMBER,
  STATE_STRING,
  STATE_UNDEFINED,
} xre_state_e;

typedef struct xre_frame_s xre_frame_t;

#define STATE_UNDEFINED (1UL << 0)
#define STATE_NUMBER (1UL << 1)
#define STATE_STRING (1UL << 2)
#define STATE_ARRAY (1UL << 3)
#define REFERENCE_FLAG (1UL << 16)

#define IS_FLAG_SET(state, flag) (((state).attrs) & (flag))
#define IS_REF_STATE(state) (IS_FLAG_SET(state, REFERENCE_FLAG))

#define REMOVE_FLAG(state, flag) ((state.attrs) &= ~(flag))
#define ADD_FLAG(state, flag) ((state.attrs) |= (flag))

#define COMPARE_FLAGS(state1, state2)                                          \
  ((REMOVE_FLAG(state1, REFERENCE_FLAG) ==                                     \
    REMOVE_FLAG(state2, REFERENCE_FLAG))                                       \
       ? 0                                                                     \
       : 1)
typedef struct {
  xre_state_e attrs;
  union {
    int64_t value;
    array_t *array;
    char *string;
  };
} xre_state_t;

typedef struct xre_frame_s {
  xre_expr_kind_t kind;
  xre_token_t *token;
  const char *identifier;
  xre_state_t state;
  int left_index;
  int right_index;
} xre_frame_t;

extern xre_frame_t *frame_tree_g;

#define LEFT_CHILD(frame) (&frame_tree_g[frame->left_index])
#define RIGHT_CHILD(frame) (&frame_tree_g[frame->right_index])
#define HAS_LEFT_CHILD(frame) (frame->left_index != -1)
#define HAS_RIGHT_CHILD(frame) (frame->right_index != -1)

int init_frame_tree(xre_ast_t *ast);
void deinit_frame_tree(xre_frame_t *frame);

void state_free(xre_state_t *state);
void state_print(xre_frame_t *frame);
bool is_true_state(xre_frame_t *frame);
const char *state_to_str(xre_state_t *state);
void state_debug(xre_frame_t *frame);

extern array_t *symtab;
extern size_t  st_read_count;
extern size_t  st_write_count;
typedef struct {
  const char *key;
  xre_state_t state;
} symtab_entry_t;



bool symtab_init(void);
void symtab_deinit(void);
symtab_entry_t *symtab_get(const char *key);
bool symtab_set(xre_frame_t *frame, const char *key, xre_state_t state);

extern bool _has_error;
extern error_notification_t _error;

bool set_error(xre_frame_t *node, error_type_e type);
bool error_occurred(void);

bool state_value(xre_frame_t *frame, int64_t value);
bool state_array_ref(xre_frame_t *frame, array_t *array);
bool state_array(xre_frame_t *frame, array_t *array);
bool state_string_ref(xre_frame_t *frame, char *string);
bool state_string(xre_frame_t *frame, char *string);
bool state_copy_ref(xre_frame_t *this, xre_frame_t *that);

bool call_runtime(xre_ast_t *ast);
bool xre_runtime(void);
bool evaluate(xre_frame_t *frame);

#endif /* __XRE_RUNTIME_H__ */
