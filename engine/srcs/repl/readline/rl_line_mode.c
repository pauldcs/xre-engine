#include "xre_readline.h"
#include "xre_repl.h"
#include "array.h"
#include <stdbool.h>

static char *history_get_index(const history_array_t *history, int index)
{
	if (index < (int)array_size(history)) {
		return (*(char **)array_at(history, index));
	}
	return (NULL);
}

static void history_move_cursor(const history_array_t *history, int depth)
{
	char *str = history_get_index(history, depth);
	if (str) {
		rl_replace_buffer(str);
	}
}

bool handle_line_mode_char(int ch, const history_array_t *history)
{
	static int history_depth = 0;

	switch ((int)ch) {
	case RL_KEY_NEWLINE:
		history_depth = 0;
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
		if (array_size(history)) {
			history_move_cursor(history, history_depth);
			history_depth = rl_clamp(history_depth + 1, 0,
						 array_size(history) - 1);
		}

		break;

	case RL_KEY_DOWN:
		history_depth =
			rl_clamp(history_depth - 1, 0, array_size(history) - 1);
		history_move_cursor(history, history_depth);

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
