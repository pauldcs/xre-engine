#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"

// BITWISE AND OPERATION
static frame_block_t *bitwise_and_op(frame_block_t *lv, frame_block_t *rv) {

  lv->_data.value = lv->_data.value & rv->_data.value;
  frame_block_free(&rv);
  rv->_src = NULL;
  return (lv);
}

// BITWISE OR OPERATION
static frame_block_t *bitwise_or_op(frame_block_t *lv, frame_block_t *rv) {

  lv->_data.value = lv->_data.value | rv->_data.value;
  frame_block_free(&rv);
  rv->_src = NULL;
  return (lv);
}

// BITWISE XOR OPERATION
static frame_block_t *bitwise_xor_op(frame_block_t *lv, frame_block_t *rv) {

  lv->_data.value = lv->_data.value ^ rv->_data.value;
  frame_block_free(&rv);
  rv->_src = NULL;
  return (lv);
}

// BITWISE LSHIFT OPERATION
static frame_block_t *bitwise_lshift_op(frame_block_t *lv, frame_block_t *rv) {
  if (rv->_data.value < 0) {
    frame_block_free(&lv);
    return error_block_with(rv, XRE_VALUE_ERROR, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (rv->_data.value > 64) {
    frame_block_free(&lv);
    return error_block_with(rv, XRE_VALUE_ERROR, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  return value_block_with(rv, lv->_data.value << rv->_data.value);
}

// BITWISE RSHIFT OPERATION
static frame_block_t *bitwise_rshift_op(frame_block_t *lv, frame_block_t *rv) {
  if (rv->_data.value < 0) {
    frame_block_free(&lv);
    return error_block_with(rv, XRE_VALUE_ERROR, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (rv->_data.value > 64) {
    frame_block_free(&lv);
    frame_block_free(&rv);
    return error_block_with(rv, XRE_VALUE_ERROR, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  lv->_data.value = lv->_data.value >> rv->_data.value;
  frame_block_free(&rv);
  rv->_src = NULL;
  return (lv);
}

// BITWISE OPERAITON
frame_block_t *bitwise_op(t_xre_expr_kind kind, frame_block_t *lv,
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

  if (!(lv->_type & IF_INTEGER)) {
    frame_block_free(&lv);
    return error_block_with(rv, XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND);
  }

  if (lv->_type != rv->_type) {
    frame_block_free(&lv);
    return error_block_with(rv, XRE_TYPE_ERROR, XRE_TYPE_MISSMATCH_ERROR);
  }

  switch (kind) {
  case __BAND__:
  case __AND_ASSIGN__:
    return (bitwise_and_op(lv, rv));
  case __BOR__:
  case __OR_ASSIGN__:
    return (bitwise_or_op(lv, rv));
  case __BXOR__:
    return (bitwise_xor_op(lv, rv));
  case __LSHIFT__:
  case __LSHIFT_ASSIGN__:
    return (bitwise_lshift_op(lv, rv));
  case __RSHIFT__:
  case __RSHIFT_ASSIGN__:
    return (bitwise_rshift_op(lv, rv));
  default:
    XRE_LOGGER(error, "Unrecognized arithmetic operation");
  }

  frame_block_free(&lv);
  return (error_block_with(rv, XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR));
}
