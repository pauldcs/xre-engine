#include "xre_readline.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct sigaction o_sigint;
struct sigaction o_sigterm;
struct sigaction o_sigtstp;
struct sigaction o_sigttou;
struct sigaction o_sigalrm;
struct sigaction o_sigquit;
struct sigaction o_sigcont;
// struct sigaction o_sigwinch;

static bool setup_signal_handler(int signal, void (*handler)(int))
{
	struct sigaction new_handler_info;
	struct sigaction old_handler_info;

	bzero(&new_handler_info, sizeof(struct sigaction));
	bzero(&old_handler_info, sizeof(struct sigaction));

	new_handler_info.sa_handler = (void (*)(int))handler;
	sigemptyset(&new_handler_info.sa_mask);

	if (sigaction(signal, &new_handler_info, &old_handler_info) != 0)
		return (false);
	return (true);
}

static void _signal_handler(int sig)
{
	switch (sig) {
	case SIGINT:
	case SIGTSTP:
		break;

		// case SIGWINCH:
		//     break;

	case SIGTERM:
	case SIGTTOU:
	case SIGALRM:
	case SIGQUIT:
	case SIGABRT:
		(void)write(STDERR_FILENO, "__DEAD__\r\n", 10);
		exit(EXIT_FAILURE);
	}
}

bool xre_repl_sigset_default(void)
{
	return (setup_signal_handler(SIGINT, _signal_handler) &&
		setup_signal_handler(SIGTERM, _signal_handler) &&
		setup_signal_handler(SIGTSTP, _signal_handler) &&
		setup_signal_handler(SIGTTOU, _signal_handler) &&
		setup_signal_handler(SIGALRM, _signal_handler) &&
		setup_signal_handler(SIGABRT, _signal_handler) &&
		setup_signal_handler(SIGQUIT, _signal_handler)
		/* && setup_signal_handler(SIGWINCH, __rl_signal_handler) */);
}

void xre_repl_clear_signals(void)
{
	(void)sigaction(SIGINT, &o_sigint, NULL);
	(void)sigaction(SIGTERM, &o_sigterm, NULL);
	(void)sigaction(SIGTSTP, &o_sigtstp, NULL);
	(void)sigaction(SIGTTOU, &o_sigttou, NULL);
	(void)sigaction(SIGALRM, &o_sigalrm, NULL);
	(void)sigaction(SIGQUIT, &o_sigquit, NULL);
	(void)sigaction(SIGABRT, &o_sigquit, NULL);
	//(void)sigaction(SIGWINCH, &o_sigwinch, NULL);
}
