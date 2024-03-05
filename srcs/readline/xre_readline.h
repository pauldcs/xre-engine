#ifndef __XRE_READLINE_H__
# define __XRE_READLINE_H__

# include "array.h"
# include <termios.h>
# include <unistd.h>
# include <stddef.h>
# include <stdint.h>

# define CTRL_KEY(k) ((k) & 0x1f)
# define DEFAULT_LINE_SIZE 64

extern bool __terminal_in_raw_mode__;
extern bool __is_termcap_inited__;

typedef struct rl_config {
  struct termios orig_termios;
  size_t screenrows;
  size_t screencols;
} t_rl_config;

extern t_rl_config config;

typedef struct {
  int x;
  int y;
} t_screen_pt;

typedef struct {
  array_t *_v;
  bool eof_reached;
  char *mode;
  int _i;  /* current index */
  int _ei; /* end index */
  int _mei; /* mac end index */
  t_screen_pt _pt; /* point on the screen were we start printing */
  t_screen_pt _cc; /* cursor x and cursor y relative to window */
} t_rl_state;

extern t_rl_state __state__;

typedef struct {
  void   *vec;
  size_t line_size;
} t_rl;

struct _tc_string {
  const char *tc_var;
  char **tc_value;
  size_t tc_len;
};

extern char *_term_clreol;
extern char *_term_clrpag;
extern char *_term_cr;
extern char *_term_backspace;
extern char *_term_goto;
extern char *_term_pc;
extern char *_term_im;
extern char *_term_ei;
extern char *_term_ic;
extern char *_term_ip;
extern char *_term_IC;
extern char *_term_dc;
extern char *_term_cd;
extern char *_term_DC;
extern char *_term_forward_char;
extern char *_term_up;
extern char *_term_UP;
extern char *_visible_bell;
extern char *_term_mm;
extern char *_term_mo;
extern char *_term_ku;
extern char *_term_kd;
extern char *_term_kr;
extern char *_term_kl;
extern char *_term_ks;
extern char *_term_ke;
extern char *_term_kh;
extern char *_term_kH;
extern char *_term_at7;
extern char *_term_kD;
extern char *_term_kI;
extern char *_term_vs;	
extern char *_term_ve;
extern char *_term_vi;

extern int _term_autowrap;

extern struct _tc_string tc_strings[32]; 

#define _XA 0x200 /* extra alphabetic */
#define _XS 0x100 /* extra space */
#define _BB  0x80 /* BEL, BS, etc. */
#define _CN  0x40 /* CR, FF, HT, NL, VT */
#define _DI  0x20 /* '0'-'9' */
#define _LO  0x10 /* 'a'-'z' */
#define _PU  0x08 /* punctuation */
#define _SP  0x04 /* space */
#define _UP  0x02 /* 'A'-'Z' */
#define _XD  0x01 /* '0'-'9', 'A'-'F', 'a'-'f' */

#define XDI (_DI|_XD)
#define XLO (_LO|_XD)
#define XUP (_UP|_XD)

#define XC_ALNUM(c)	(_CharType[(int)(c)] & (_DI|_LO|_UP|_XA))
#define XC_ALPHA(c)	(_CharType[(int)(c)] & (_LO|_UP|_XA))
#define XC_CNTRL(c)	(_CharType[(int)(c)] & (_BB|_CN))
#define XC_DIGIT(c)	(_CharType[(int)(c)] & _DI)
#define XC_GRAPH(c)	(_CharType[(int)(c)] & (_DI|_LO|_PU|_UP|_XA))
#define XC_LOWER(c)	(_CharType[(int)(c)] & _LO)
#define XC_PRINT(c)	(_CharType[(int)(c)] & (_DI|_LO|_PU|_SP|_UP|_XA))
#define XC_PUNCT(c)	(_CharType[(int)(c)] & _PU)
#define XC_SPACE(c)	(_CharType[(int)(c)] & (_CN|_SP|_XS))
#define XC_UPPER(c)	(_CharType[(int)(c)] & _UP)
#define XC_XDIGIT(c) (_CharType[(int)(c)] & _XD)

extern const uint16_t _CharType[256];

# define NUM_TC_STRINGS (sizeof (tc_strings) / sizeof (struct _tc_string))

typedef enum {
    RL_KEY_NONE         = 0,
    RL_KEY_EOF          = 2,
    RL_KEY_UP           = 3,
    RL_KEY_DOWN         = 4,
    RL_KEY_LEFT         = 5,
    RL_KEY_RIGHT        = 6,
    RL_KEY_DEL          = 7,
    RL_KEY_HOME         = 8,
    RL_KEY_END          = 9,
    RL_KEY_WORDLEFT     = 10,
    RL_KEY_WORDRIGHT    = 11,
    RL_KEY_ADDLINE      = 12,
    RL_KEY_REMLINE      = 13,
    RL_KEY_DEL_INPLACE  = 14,
    RL_KEY_CLEAR_SCREEN = 15,
    RL_KEY_START_EDITOR = 16,
    RL_KEY_NEWLINE      = 17,
    RL_KEY_INTR         = 18,
} rl_keys;

/*    TERMINAL
 */
bool rl_raw_mode_enable(void);
bool rl_raw_mode_disable(void);
bool rl_init_termcaps(void);
void rl_free_termcaps(void);
void rl_get_terminal_caps(void);
bool rl_get_window_size(size_t *rows, size_t *cols);

/*    READLINE
 */
ssize_t xre_readline(char **buf, array_t *history);
int try_read_key_fd(int fildes);
bool handle_line_mode_char(int ch);
bool handle_editor_mode_char(char ch);

/*    UTILS
 */
void    rl_set_cursor(bool visible);
size_t  rl_esc_to_str(char *s);
void	  rl_putnbr(const int32_t c);
int     rl_xputchar(int c);
ssize_t rl_xwrite(int fildes, const char *buf, size_t nbytes);
ssize_t rl_xgetchr(int fildes);


/*    ACTIONS
 */
void    rl_clear_screen(void);
void    rl_cr(void);
void    rl_move_cursor_down(size_t n);
void    rl_move_cursor_up(size_t n);
void    rl_move_cursor_left(size_t n);
void    rl_move_cursor_right(size_t n);
void    rl_move_cursor_to(size_t x, size_t y);
void    rl_scroll_forward(size_t n);
void    rl_scroll_backward(size_t n);
void    rl_cursor_forward(void);
void    rl_cursor_backward(void);

/*    DISPLAY
 */
bool    rl_redisplay(void);
bool    rl_redisplay_editor(void);
void    rl_erase_line(void);
void    rl_display_status(const char *mode);
void    rl_clear_status(void);

/*    BUFFER
*/
bool rl_buf_add_nullterm(void);
bool rl_buf_inject_at(size_t at, void *data, size_t size);
void rl_buf_delete_at(size_t at, size_t size);
void rl_discard_line(void);

#endif /* __XRE_READLINE_H__ */
