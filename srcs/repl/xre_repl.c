#include "xre_repl.h"
#include "array.h"
#include "xre_parse.h"
#include "xre_readline.h"
#include "xre_runtime.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

bool xre_process_block(const char *block) {

  if (!block || !*block)
    return (true);

  xre_ast_t *ast = xre_ast_compose(block);
  if (ast) {
    (void)call_runtime(ast);
    ast_free(ast);
    return (true);
  }

  return (false);
}

// void print_hist(array_t *hist) {

//   size_t size = array_size(hist);
//   for (size_t i = 0; i < size; i++) {
//     printf(" - '%s'\n", *(char **)array_at(hist, i));
//   }
// }

bool xre_repl_entrypoint(void) {

  /*
   *   Load the history from a file
   */
  //array_t *hist = array_create(sizeof(char *), 10, NULL);
  void *buffer;
  // if (!hist)
  //   return (false);

  (void)xre_repl_sigset_default();

  for (;;) {
    if (!xre_read_block(&buffer, NULL)) {
      xre_repl_clear_signals();
      return (/*array_kill(hist),*/ false);
    }
    
    (void)xre_process_block(buffer);
    
    free(buffer);
    /*if (!array_push(hist, &buffer)) {
      return (array_kill(hist), free(buffer), false);
    }
    array_kill(hist);
    // print_hist(hist);
    */
  }

  free(buffer);
  xre_repl_clear_signals();
  return (true);
}
