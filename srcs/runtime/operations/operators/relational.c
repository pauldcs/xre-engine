#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"
#include <stdbool.h>

static inline int value_cmp(int64_t a, int64_t b) {
  if (a < b)
    return (-1);
  if (a > b)
    return (1);
  return (0);
}

static inline int array_cmp(const array_t *a, const array_t *b) {
  size_t size_a = array_size(a);
  size_t size_b = array_size(b);

  if (value_cmp(size_a, size_b))
    return (value_cmp(size_a, size_b));

  return (memcmp(array_at(a, 0), array_at(b, 0), size_a));
}

// RELATIONAL OPERAITON
bool relational_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left) || !evaluate(right)) {
    return (false);
  }

  bool matching_types = left->state.type == right->state.type;

  int equality = 0;

  if (matching_types) {
    if (left->state.type == STATE_NUMBER) {
      equality = value_cmp(left->state.value, right->state.value);
    
    } else if (left->state.type == STATE_STRING) {
      equality = strcmp(left->state.string, right->state.string);
    
    } else if (left->state.type == STATE_ARRAY) {
      equality = array_cmp(left->state.array, right->state.array);
    }
  }

  if (frame->kind == __EQ__) {
    return (state_value(frame, matching_types && equality == 0));

  } else if (frame->kind == __NE__) {
    return (state_value(frame, !matching_types || equality != 0));

  } else {
    if (matching_types) {
      switch (frame->kind) {
      case __LT__:
        return (state_value(frame, equality < 0));
      case __GT__:
        return (state_value(frame, equality > 0));
      case __LE__:
        return (state_value(frame, equality <= 0));
      case __GE__:
        return (state_value(frame, equality >= 0));
      default:
        break;
      }
    }
  
    __return_error(frame, XRE_TYPE_MISSMATCH_ERROR);
  }
}
