#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_parse.h"

bool not_op(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  xre_ast_t *uni = node->uniop;

  if (!evaluate(uni)) {
    return (false);
  }

  return (change_state_value(node, !is_truthy_state(uni)));
}
