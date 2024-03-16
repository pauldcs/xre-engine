#include "xre_parse.h"
#include "xre_runtime.h"

size_t get_ast_size(xre_ast_t *ast) {
  size_t size = 1;
  
  switch (ast->kind) {
  case __VAL__:
  case __STRING_LITERAL__:
  case __IDENTIFIER__:
    return (size);

  case __NOT__:
  case __PRINT__:
    return (size
      + get_ast_size(ast->_binop.left));
  
  default:
    return (size
      + get_ast_size(ast->_binop.left)
      + get_ast_size(ast->_binop.right));
  }
}