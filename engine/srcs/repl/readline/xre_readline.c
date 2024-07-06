#include "xre_readline.h"
#include "array.h"
#include "xre_assert.h"
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ttydefaults.h>
#include <termcap.h>
#include <unistd.h>

static bool get_window_size(void)
{
	return (rl_get_window_size(&config.screenrows, &config.screencols));
}

bool rl_get_input(void)
{
	bool editor_requested = false;
	__state__.mode = "NORM";
	int ch;

	(void)write(STDOUT_FILENO, "\r\n", 2);

	while (true) {
		if (!get_window_size() || !rl_redisplay())
			return (false);

try_read_key:

		switch ((ch = try_read_key_fd(STDIN_FILENO))) {
		case -1:
			return (false);
		case 0:
			/*
         *    We go back to the try_read_key label here because
         *    read has a timeout and returns 0 very often
         */
			goto try_read_key;

		case RL_KEY_START_EDITOR:
			editor_requested = true;
			__state__.mode = "EDITOR";
			break;

		default:
			if (editor_requested) {
				if (!handle_editor_mode_char(ch))
					return (false);

			} else if (!handle_line_mode_char(ch)) {
				return (false);
			}

			if (__state__.eof_reached)
				return (true);
			break;
		}
	}
	return (true);
}

ssize_t xre_readline(char **buf, array_t *history)
{
	__return_val_if_fail__(buf, -1);

	(void)history;
	size_t bufsize;

	memset(&__state__, 0, sizeof(__state__));

	__state__._v = array_create(sizeof(char), 64, NULL);
	if (!__state__._v)
		return (false);

	if (!isatty(STDIN_FILENO)) {
		(void)rl_xwrite(STDERR_FILENO,
				"error: STDIN_FILENO: Is not a tty\r\n", 35);
		goto prison;
	}

	if (!rl_init_termcaps() || !rl_raw_mode_enable() || !rl_get_input())
		goto prison;

	/*
   * clean up screen
   */
	rl_clear_status();
	(void)rl_raw_mode_disable();
	bufsize = array_size(__state__._v);
	*buf = array_extract(__state__._v, 0, bufsize);
	array_kill(__state__._v);
	return (bufsize);

prison:
	rl_clear_status();
	(void)rl_raw_mode_disable();
	rl_discard_line();
	*buf = NULL;
	return (-1);
}
