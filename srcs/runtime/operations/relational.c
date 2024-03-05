#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include <stdbool.h>

// VALUE CMP
static int value_cmp(int64_t a, int64_t b) {
  if (a < b)
    return (-1);
  if (a > b)
    return (1);
  return (0);
}

// ARRAY CMP
static int array_cmp(const array_t *a, const array_t *b) {
  size_t size_a = array_size(a);
  size_t size_b = array_size(b);

  if (value_cmp(size_a, size_b))
    return (value_cmp(size_a, size_b));

  return (memcmp(array_at(a, 0), array_at(b, 0), size_a));
}

// RELATIONAL OPERAITON
frame_block_t *relational_op(t_xre_expr_kind kind, frame_block_t *lv,
                             frame_block_t *rv) {
  __return_val_if_fail__(lv, NULL);
  __return_val_if_fail__(rv, NULL);

  if (lv->_error != NULL) {
    frame_block_free(&rv);
    return (lv);
  }

  if (rv->_error != NULL) {
    frame_block_free(&lv);
    return (rv);
  }

  bool matching_types = lv->_type == rv->_type;
  int equality = 0;

  if (matching_types) {
    if (lv->_type & IF_INTEGER)
      equality = value_cmp(lv->_data.value, rv->_data.value);
    else
      equality = array_cmp(lv->_data.array, rv->_data.array);
  }

  frame_block_free(&rv);
  frame_block_free(&lv);

  if (kind == __EQ__) {
    return ((matching_types && equality == 0) ? true_block_alloc() : false_block_alloc());

  } else if (kind == __NE__) {
    return ((!matching_types || equality != 0) ? true_block_alloc() : false_block_alloc());

  } else {
    if (matching_types) {
      switch (kind) {
      case __LT__:
        return (equality < 0 ? true_block_alloc() : false_block_alloc());
      case __GT__:
        return (equality > 0 ? true_block_alloc() : false_block_alloc());
      case __LE__:
        return (equality <= 0 ? true_block_alloc() : false_block_alloc());
      case __GE__:
        return (equality >= 0 ? true_block_alloc() : false_block_alloc());
      default:
        break;
      }
    }
    return (error_block_alloc(XRE_TYPE_ERROR, XRE_TYPE_MISSMATCH_ERROR));
  }

  XRE_LOGGER(error, "Unrecognized comparison");
  return (error_block_alloc(XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR));
}
