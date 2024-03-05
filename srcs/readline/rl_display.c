#include "xre_readline.h"
#include "array.h"
#include "xre_stringf.h"
#include <stdbool.h>
#include <stddef.h>
#include <termcap.h>
#include <string.h>

void
rl_display_status(const char *mode) {

  static char screen[512];

  rl_move_cursor_to(0, config.screenrows);
  rl_erase_line();

  (void)mode;
  slcpyf(screen, config.screencols - 1,
    " xre-engine - v0.5 ..."
  );              
  fputstr(1, "%s%s%s", "\033[0;93m", screen, "\033[0m");
  rl_move_cursor_to(__state__._pt.x, __state__._pt.y);
}

void
rl_clear_status(void) {
  rl_move_cursor_to(0, config.screenrows - 1);
  rl_erase_line();
}

size_t
str_count_char(const char *str, int c) {

  size_t n = 0;
  while (*str) {
    if (*(str++) == c)
      n++;
  }
  return (n);
}

static int
get_num_lines(int string_size, int terminal_x, int terminal_y) {

  int num_lines = string_size / terminal_x;
  int remaining_chars = string_size % terminal_x;

  if (remaining_chars > 0)
      num_lines++;

  return num_lines > terminal_y ? terminal_y : num_lines;
}

bool
rl_redisplay(void) {
  char screen[4096];

  xmemset(screen, ' ', __state__._mei);

  rl_set_cursor(false);
  rl_erase_line();

  if ((__state__._mei + 3) % config.screencols == 0) {
    rl_move_cursor_to(0, config.screenrows);
    rl_scroll_forward(1);
  }

  __state__._pt.x = 0;
  __state__._pt.y = config.screenrows
      - get_num_lines(
        __state__._mei + 3,
        config.screencols,
        config.screenrows
      );
  __state__._pt.y -= 1;

  rl_move_cursor_to(0, __state__._pt.y);
  
  char *str = tgetstr("cd", NULL);
  size_t len = strlen(str);

  xmemcpy(screen, str, len);
  xmemcpy(&screen[len], "%> ", 3);
  xmemcpy(
    &screen[len + 3],
    array_unsafe_access(__state__._v, 0),
    __state__._ei
  );

  write(1, screen, __state__._ei + len + 3);

  __state__._cc.x = __state__._pt.x;
  __state__._cc.y = __state__._pt.y;

  __state__._cc.x += (__state__._i + 3) % config.screencols;
  __state__._cc.y += (__state__._i + 3) / config.screencols;
    
  rl_display_status(__state__.mode);
  rl_move_cursor_to(__state__._cc.x, __state__._cc.y);
  rl_set_cursor(true);
  return (true);
}
