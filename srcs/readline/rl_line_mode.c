#include "xre_readline.h"
#include <stdbool.h>

bool handle_line_mode_char(int ch)
{
	switch ((int)ch) {
	case RL_KEY_NEWLINE:
		if (rl_buf_add_nullterm()) {
			__state__.eof_reached = true;
			return (true);
		}
		return (false);
		break;

	case RL_KEY_INTR:
		array_clear(__state__._v);
		__state__.eof_reached = true;
		return (true);

	case RL_KEY_EOF:

		break;
	case RL_KEY_HOME:
		__state__._i = 0;

		break;
	case RL_KEY_END:
		__state__._i = __state__._ei;

		break;
	case RL_KEY_WORDLEFT:

		break;
	case RL_KEY_WORDRIGHT:

		break;
	case RL_KEY_ADDLINE:

		break;
	case RL_KEY_REMLINE:

		break;
	case RL_KEY_LEFT:
		if (__state__._i)
			__state__._i--;

		break;
	case RL_KEY_RIGHT:
		if (__state__._i != __state__._ei)
			__state__._i++;

		break;
	case RL_KEY_CLEAR_SCREEN:
		rl_clear_screen();

		break;
	case RL_KEY_DEL:
		if (__state__._i)
			rl_buf_delete_at(__state__._i, 1);

		break;
	case RL_KEY_UP:
		if (__state__._i)
			break;
		// get history up
		break;

	case RL_KEY_DOWN:
		if (__state__._i)
			break;
		// get history down
		break;

	default:
		if (!ch)
			break;

		if (!rl_buf_inject_at(__state__._i, &ch, 1))
			return (false);
		break;
	}
	return (true);
}
