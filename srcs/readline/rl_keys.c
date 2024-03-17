#include "xre_readline.h"
#include <stdbool.h>
#include <unistd.h>

int try_read_key_fd(int fildes)
{
	int8_t ch;

	switch ((ch = rl_xgetchr(fildes))) {
	case -1:
		return (-1);
	case 0:
		return (0);

	case CEOF:
		/*
     *    In my world Ctrl-D is an error
     */
		return (-1);

	case CINTR:
		(void)write(STDERR_FILENO, "__CINTR__", 10);
		/*
     *    If ctrl-C is pressed, we abort the line by
     *    setting it's the size to zero. What is
     *    in the line buffer should be considered
     *    undefined
     */
		return (RL_KEY_INTR);

	case CTRL('s'):
		return (RL_KEY_EOF);
	case CTRL('a'):
		return (RL_KEY_HOME);
	case CTRL('e'):
		return (RL_KEY_END);
	case CTRL('b'):
		return (RL_KEY_WORDLEFT);
	case CTRL('n'):
		return (RL_KEY_WORDRIGHT);
	case CTRL('w'):
		return (RL_KEY_ADDLINE);
	case CTRL('x'):
		return (RL_KEY_REMLINE);
	case CTRL('l'):
		return (RL_KEY_CLEAR_SCREEN);
	case CTRL('p'):
		return (RL_KEY_START_EDITOR);

	case CERASE:
		return (RL_KEY_DEL);

	case '\r':
		/*
     *    '\r' was recieved, which means '\n'
     *    because we are in raw mode
     *    todo: send only when not in
     *    multiline mode
    //  */
		return (RL_KEY_NEWLINE);

	case '\t':
	case '\n':
		/*
     *    if in multiline mode,
     *    we add those to the array
     */

	case '\v':
		/*
     *    do not accept any space characters
     */
		return (0);

	case 27:
		/*
     *    We recieved ESC so we
     *    try to see if is just ESC or
     *    a full escape sequence
     */
		switch ((ch = rl_xgetchr(fildes))) {
		case -1:
			return (-1);

		case '[':
			/*
       *    We got into the beginning of what seems
       *    to be a valid escape sequence
       */
			switch ((ch = rl_xgetchr(fildes))) {
			case -1:
				return (-1);
			case 'A':
				return (RL_KEY_UP);
			case 'B':
				return (RL_KEY_DOWN);
			case 'D':
				return (RL_KEY_LEFT);
			case 'C':
				return (RL_KEY_RIGHT);
			/*
       *    no default here so that it falls
       *    into the lower one, will implement
       *    more sequences later
       */
			default:
				return (0);
			}
		/*
     *   if we could not parse the sequence in a
     *   meaningful way, we return 0 so that the
     *   sequence is descarded
     */
		default:
			return (0);
		}
	/*
   *    If it did not fall into any of
   *    the cases it is just a regular
   *    character and we skip it if it
   *    is non printable
   */
	default:
		if (ch < 0 || !XC_PRINT(ch))
			return (0);

		return (ch);
	}
}
