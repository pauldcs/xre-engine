#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"

// ADD OPERATION
frame_block_t *add_op(frame_block_t *lv, frame_block_t *rv) {
  // if (lv->_data.value < 0) {
  //   if (!I64_IS_SUBABBLE(lv->_data.value, -rv->_data.value)) {
  //     return error_block_alloc(XRE_ARITHMETIC_ERROR, XRE_UNDERFLOW_ERROR);
  //   }
  // }

  // if (!I64_IS_ADDABBLE(lv->_data.value, rv->_data.value)) {
  //   return error_block_alloc(XRE_ARITHMETIC_ERROR, XRE_OVERFLOW_ERROR);
  // }

  lv->_data.value = lv->_data.value + rv->_data.value;
  frame_block_free(&rv);
  return (lv);
}

// SUB OPERATION
frame_block_t *sub_op(frame_block_t *lv, frame_block_t *rv) {
  // if (lv->_data.value < 0) {
  //   if (!I64_IS_ADDABBLE(lv->_data.value, -rv->_data.value)) {
  //     return error_block_alloc(XRE_ARITHMETIC_ERROR, XRE_UNDERFLOW_ERROR);
  //   }
  // }

  // if (!I64_IS_SUBABBLE(lv->_data.value, rv->_data.value)) {
  //   return error_block_alloc(XRE_ARITHMETIC_ERROR, XRE_OVERFLOW_ERROR);
  // }

  lv->_data.value = lv->_data.value - rv->_data.value;
  frame_block_free(&rv);
  return (lv);
}

// MUL OPERATION
frame_block_t *mul_op(frame_block_t *lv, frame_block_t *rv) {

  // if (!I64_IS_MULABBLE(lv->_data.value, rv->_data.value)) {
  //   return error_block_alloc(XRE_ARITHMETIC_ERROR, XRE_OVERFLOW_ERROR);
  // }

  lv->_data.value = lv->_data.value * rv->_data.value;
  frame_block_free(&rv);
  return (lv);
}

// DIV OPERATION
frame_block_t *div_op(frame_block_t *lv, frame_block_t *rv) {
  if (rv->_data.value == 0) {
    frame_block_free(&lv);
    return error_block_with(rv, XRE_ARITHMETIC_ERROR, XRE_ZERO_DIVISION_ERROR);
  }

  lv->_data.value = lv->_data.value / rv->_data.value;
  frame_block_free(&rv);
  return (lv);
}

// MOD OPERATION
frame_block_t *mod_op(frame_block_t *lv, frame_block_t *rv) {
  if (rv->_data.value == 0) {
    frame_block_free(&lv);
    return error_block_with(rv, XRE_ARITHMETIC_ERROR, XRE_ZERO_DIVISION_ERROR);
  }

  lv->_data.value = lv->_data.value % rv->_data.value;
  frame_block_free(&rv);
  return (lv);
}

// ARITHMETIC OPERAITON
frame_block_t* arithmetic_op(t_xre_expr_kind kind, frame_block_t* lv, frame_block_t* rv) {
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

  if (lv->_type != IF_INTEGER) {
    frame_block_free(&lv);
    return (error_block_with(rv, XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND));
  }

  if (lv->_type != rv->_type) {
    frame_block_free(&lv);
    return (error_block_with(rv, XRE_TYPE_ERROR, XRE_TYPE_MISSMATCH_ERROR));
  }

  switch (kind) {
  case __ADD__:
  case __ADD_ASSIGN__:
    return (add_op(lv, rv));
  case __SUB__:
  case __SUB_ASSIGN__:
    return (sub_op(lv, rv));
  case __MUL__:
  case __MUL_ASSIGN__:
    return (mul_op(lv, rv));
  case __DIV__:
  case __DIV_ASSIGN__:
    return (div_op(lv, rv));
  case __MOD__:
  case __MOD_ASSIGN__:
    return (mod_op(lv, rv));
  case __POW__:
  case __POW_ASSIGN__:
  default:
    XRE_LOGGER(error, "Unrecognized arithmetic operation");
  }

  frame_block_free(&lv);
  XRE_LOGGER(warning, "Confusing condition");
  return (error_block_with(rv, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
