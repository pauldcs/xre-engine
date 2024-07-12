#include "xre_assert.h"
#include "xre_readline.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termcap.h>
#include <termios.h>
#include <unistd.h>

bool rl_init_termcaps(void)
{
	char *term_type;
	int ret;

	if (__is_termcap_inited__)
		return (true);

	term_type = getenv("TERM");
	__return_val_if_fail__(term_type && "TERM must be set (see 'env')",
			       false);

	ret = tgetent(NULL, term_type);
	__return_val_if_fail__(
		ret != -1 && "The terminfo database could not be found", false);

	(void)ret;

	__return_val_if_fail__(ret != 0 && "No such entry", false);
	rl_get_terminal_caps();
	__is_termcap_inited__ = true;
	return (true);
}

void rl_get_terminal_caps(void)
{
	static char __[2048];
	for (size_t i = 0; i < NUM_TC_STRINGS; i++) {
		char **_ptr = (char **)&__;
		*(tc_strings[i].tc_value) =
			tgetstr((char *)tc_strings[i].tc_var, _ptr);

		if (*(tc_strings[i].tc_value)) {
			tc_strings[i].tc_len =
				strlen(*(tc_strings[i].tc_value));
		} /* else {
fprintf(stderr,
"Failed to get termcap for: '%s'\n", tc_strings[i].tc_var
);
} */
	}

	PC = _term_pc ? *_term_pc : 0;

	BC = _term_backspace;
	UP = _term_up;

	if (!_term_cr)
		_term_cr = "\r";

	_term_autowrap = tgetflag("am") && tgetflag("xn");
}

bool rl_raw_mode_disable(void)
{
	if (!__terminal_in_raw_mode__)
		return (true);

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.orig_termios) == -1) {
		fprintf(stderr, "tcsetattr: %s\n", strerror(errno));
		return (false);
	}
	__terminal_in_raw_mode__ = false;
	return (true);
}

bool rl_raw_mode_enable(void)
{
	if (__terminal_in_raw_mode__)
		return (true);

	struct termios raw;

	if (tcgetattr(STDIN_FILENO, &config.orig_termios) == -1) {
		fprintf(stderr, "tcgetattr: %s\n", strerror(errno));
		return (false);
	}

	raw = config.orig_termios;

	raw.c_oflag &= ~(OPOST);
	raw.c_iflag &= ~(IXON | ISIG | ICRNL);

	raw.c_lflag &= ~(ICANON | ECHO | IEXTEN);

	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
		fprintf(stderr, "tcsetattr: %s\n", strerror(errno));
		return (false);
	}

	__terminal_in_raw_mode__ = true;
	return (true);
}

bool rl_get_window_size(size_t *rows, size_t *cols)
{
	struct winsize ws;
	int ret;

	ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

	if (ret == -1) {
		fprintf(stderr, "ioctl: %s\n", strerror(errno));
	}
	if (ws.ws_col == 0) {
		return (false);
	}

	*cols = ws.ws_col;
	*rows = ws.ws_row;
	return (true);
}

/*
 *    Guarantees that the terminal goes back into
 *    cooked mode when the program exits unexpectedly
 *    (sigkill does not count)
 */
__attribute__((destructor)) void rl_at_exit_disable_raw_mode(void)
{
	if (__terminal_in_raw_mode__)
		rl_raw_mode_disable();
}
