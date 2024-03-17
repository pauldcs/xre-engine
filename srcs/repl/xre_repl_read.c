#include "xre_log.h"
#include "xre_readline.h"
#include "xre_repl.h"
#include <stdbool.h>

bool xre_read_block(void **block, array_t *history)
{
try_read:
	switch (xre_readline((char **)block, history)) {
	case 0:
		goto try_read;
	case -1: {
		XRE_LOGGER(info, "closing xre-engine ...");
		return (false);
	}
	default:
		return (true);
	}
}
