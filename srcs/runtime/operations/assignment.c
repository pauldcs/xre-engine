#include "xre_runtime.h"
#include "xre_utils.h"
#include "xre_assert.h"
#include "xre_log.h"

static frame_block_t *simple_assignment(t_xre_ast *node, frame_block_t *rv) {
  if (!runtime_stack_set(node->string, rv)
    && !runtime_stack_add(node->string, rv))
    return (copy_block_alloc(rv));

  rv->_src = NULL;
  return (rv);
}

static frame_block_t *reassignement(t_xre_ast *node, frame_block_t *rv) {
  if (runtime_stack_set(node->string, rv))
    return (copy_block_alloc(rv));
  
  frame_block_free(&rv);
  return (error_block_alloc(XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));
}

frame_block_t *assignment_op(t_xre_expr_kind kind, t_xre_ast *node, frame_block_t *rv) {
  __return_val_if_fail__(rv, NULL);
  __return_val_if_fail__(node, NULL);

  if (rv->_error != NULL) {
    return (rv);
  }
  
  if (node->kind != __IDENTIFIER__) {
    frame_block_free(&rv);
    return error_block_alloc(XRE_TYPE_ERROR, XRE_INVALID_ASSIGMENT_ERROR);
  }

  if (kind == __ASSIGN__) {
    return (simple_assignment(node, rv));
  }

  frame_block_t *block = runtime_stack_get(node->string);
  
  if (!block) {
    frame_block_free(&rv);
    return error_block_alloc(XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR);
  }

  if (block->_type != IF_INTEGER) {
    frame_block_free(&rv);
    return error_block_alloc(XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND);
  }

  switch (kind) {
  case __ADD_ASSIGN__:
    return (reassignement(node, add_op(block, rv)));
  case __SUB_ASSIGN__:
    return (reassignement(node, sub_op(block, rv)));
  case __MUL_ASSIGN__:
    return (reassignement(node, mul_op(block, rv)));
  case __DIV_ASSIGN__:
    return (reassignement(node, div_op(block, rv)));
  case __MOD_ASSIGN__:
    return (reassignement(node, mod_op(block, rv)));
  case __POW_ASSIGN__:
  default:
    XRE_LOGGER(error, "Unrecognized arithmetic operation");
  }

  frame_block_free(&rv);
  XRE_LOGGER(warning, "Confusing condition");
  return (error_block_alloc(XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
