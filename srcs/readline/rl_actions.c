#include "xre_assert.h"
#include "xre_readline.h"
#include "xre_stringf.h"
#include <termcap.h>
#include <stdio.h>
#include <string.h>

void
rl_clear_screen(void) {
  __return_if_fail__(_term_clrpag);

	(void)tputs(_term_clrpag, 1, &rl_xputchar);
}

void
rl_backspace(size_t n) {
  __return_if_fail__(_term_backspace);

	while (n--) {
		(void)tputs(_term_backspace, 1, rl_xputchar);
	}
}

void
rl_erase_line(void) {
	__return_if_fail__(_term_clreol);

	(void)tputs(_term_clreol, 1, rl_xputchar);
}

void
rl_cr(void) {
	__return_if_fail__(_term_cr);

	(void)tputs(_term_cr, 1, rl_xputchar);
}

void
rl_move_cursor_up(size_t n) {
	__return_if_fail__(_term_up && *_term_up);
  
	while (n--) {
		(void)tputs (_term_up, 1, rl_xputchar);
	}
}

void
rl_move_cursor_left(size_t n) {
	__return_if_fail__(_term_backspace);
  
	while (n--) {
		(void)tputs (_term_backspace, 1, rl_xputchar);
	}
}

void
rl_move_cursor_right(size_t n) {
	__return_if_fail__(_term_forward_char);
  
	while (n--) {
		(void)tputs (_term_forward_char, 1, rl_xputchar);
	}
}

void
rl_move_cursor_down(size_t n) {

	while (n--)
  	(void)write(STDOUT_FILENO, "\n", 1);
}

void
rl_move_cursor_to(size_t x, size_t y) {

  (void)fputstr(STDOUT_FILENO,
    tgoto(tgetstr("cm", NULL), x, y),
    1,
    &rl_xputchar);
}


void
rl_scroll_forward(size_t n) {

	while (n--) {
  	(void)tputs (tgetstr("sf", NULL), 1, rl_xputchar);
  }
}

void
rl_scroll_backward(size_t n) {

	while (n--) {
  	(void)tputs (tgetstr("sb", NULL), 1, rl_xputchar);
  }
}

void
rl_set_cursor(bool visible) {
	__return_if_fail__(_term_ve);
	__return_if_fail__(_term_vi);

	if (visible) (void)tputs(_term_ve, 1, &rl_xputchar);
	else         (void)tputs(_term_vi, 1, &rl_xputchar);
}
