#include "array.h"
#include "xre_readline.h"

void *__screen__ = NULL;

bool __terminal_in_raw_mode__ = false;
bool __is_termcap_inited__ = false;

int _term_autowrap = -1;

t_rl_config config;

t_rl_state __state__ = {
	._i = 0, // index
	._ei = 0, // end index
	._mei = 0, // max end index
	._cc.x = 0, // current cursor x
	._cc.y = 0 // current cursor y
};

char *_term_clreol;
char *_term_clrpag;
char *_term_cr;
char *_term_backspace;
char *_term_goto;
char *_term_pc;
char *_term_im;
char *_term_ei;
char *_term_ic;
char *_term_ip;
char *_term_IC;
char *_term_dc;
char *_term_cd;
char *_term_DC;
char *_term_forward_char;
char *_term_up;
char *_term_UP;
char *_visible_bell;
char *_term_mm;
char *_term_mo;
char *_term_ku;
char *_term_kd;
char *_term_kr;
char *_term_kl;
char *_term_ks;
char *_term_ke;
char *_term_kh;
char *_term_kH;
char *_term_at7;
char *_term_kD;
char *_term_kI;
char *_term_vs;
char *_term_ve;
char *_term_vi;

const uint16_t _CharType[256] = {
	0,   _BB, _BB, _BB, _BB, _BB, _BB, _BB, _BB, _CN, _CN, _CN, _CN,
	_CN, _BB, _BB, _BB, _BB, _BB, _BB, _BB, _BB, _BB, _BB, _BB, _BB,
	_BB, _BB, _BB, _BB, _BB, _BB, _SP, _PU, _PU, _PU, _PU, _PU, _PU,
	_PU, _PU, _PU, _PU, _PU, _PU, _PU, _PU, _PU, XDI, XDI, XDI, XDI,
	XDI, XDI, XDI, XDI, XDI, XDI, _PU, _PU, _PU, _PU, _PU, _PU, _PU,
	XUP, XUP, XUP, XUP, XUP, XUP, _UP, _UP, _UP, _UP, _UP, _UP, _UP,
	_UP, _UP, _UP, _UP, _UP, _UP, _UP, _UP, _UP, _UP, _UP, _UP, _UP,
	_PU, _PU, _PU, _PU, _PU, _PU, XLO, XLO, XLO, XLO, XLO, XLO, _LO,
	_LO, _LO, _LO, _LO, _LO, _LO, _LO, _LO, _LO, _LO, _LO, _LO, _LO,
	_LO, _LO, _LO, _LO, _LO, _LO, _PU, _PU, _PU, _PU, _BB,
};

struct _tc_string tc_strings[32] = {
	{ "@7", &_term_at7, 0 },
	{ "DC", &_term_DC,
	  0 }, /* String of commands to delete n characters starting
   at the cursor. It has the same effect as repeating the `dc' string n times.
   Any terminal description that has `DC' also has `dc'.*/
	{ "IC", &_term_IC,
	  0 }, /* String of commands to insert n character positions
   at and after the cursor. It has the same effect as repeating the `ic' string
   and a space, n times. If `IC' is provided, application programs may use it
   without first entering insert mode. */
	{ "ce", &_term_clreol,
	  0 }, /* String of commands to clear from the cursor to
                                 the end of the current line. */
	{ "cl", &_term_clrpag, 0 }, /* String to clear the entire screen and put
                                 cursor at upper left corner. */
	{ "cr", &_term_cr,
	  0 }, /* String to move cursor sideways to left margin. */
	{ "dc", &_term_dc,
	  0 }, /* String to delete one character position at the cursor. */
	{ "cd", &_term_cd,
	  0 }, /* String to delete everything after the cursor and lines below. */
	{ "ei", &_term_ei, 0 }, /* String to leave insert mode. */
	{ "ic", &_term_ic,
	  0 }, /* String to insert one character position at the cursor. */
	{ "im", &_term_im, 0 }, /* String to enter insert mode. */
	{ "kD", &_term_kD,
	  0 }, /* String of input sent by the "delete character" key. */
	{ "kH", &_term_kH,
	  0 }, /* String of input sent by the "home down" key. */
	{ "kI", &_term_kI,
	  0 }, /* String of input sent by the "insert character" or
                             "enter insert mode" key. */
	{ "kd", &_term_kd,
	  0 }, /* String of input sent by typing the down-arrow key. */
	{ "ke", &_term_ke,
	  0 }, /* String to make the function keys work locally. */
	{ "kh", &_term_kh,
	  0 }, /* String of input sent by typing the "home-position" key. */
	{ "kl", &_term_kl,
	  0 }, /* String of input sent by typing the left-arrow key. */
	{ "kr", &_term_kr,
	  0 }, /* String of input sent by typing the right-arrow key. */
	{ "ks", &_term_ks, 0 }, /* String to make the function keys transmit. */
	{ "ku", &_term_ku,
	  0 }, /* String of input sent by typing the up-arrow key. */
	{ "le", &_term_backspace,
	  0 }, /* String to move the cursor left one column. */
	{ "mm", &_term_mm,
	  0 }, /* String to enable the functioning of the Meta key. */
	{ "mo", &_term_mo,
	  0 }, /* String to disable the functioning of the Meta key. */
	{ "nd", &_term_forward_char,
	  0 }, /* String to move the cursor right one column. */
	{ "pc", &_term_pc, 0 }, /* String containing character for padding. */
	{ "up", &_term_up,
	  0 }, /* String to move the cursor vertically up one line. */
	{ "UP", &_term_UP,
	  0 }, /* String to move the cursor vertically up n lines. */
	{ "vb", &_visible_bell, 0 }, /* String to make the screen flash. */
	{ "vs", &_term_vs, 0 }, /* String to enhance the cursor. */
	{ "vi", &_term_vi, 0 }, /* String to make the cursor invisible. */
	{ "ve", &_term_ve, 0 }, /* String to return the cursor to normal. */
};
