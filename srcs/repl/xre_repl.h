#ifndef __XRE_REPL_H__
# define __XRE_REPL_H__

# include "array.h"
# include <stdbool.h>

bool xre_repl_entrypoint(void);
bool xre_read_block(void **block, array_t *history);

bool xre_repl_sigset_default(void);
void xre_repl_clear_signals(void);

#endif /* __XRE_REPL_H__ */
