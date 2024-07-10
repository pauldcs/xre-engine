#include "xre_repl.h"
#include "array.h"
#include "xre_parse.h"
#include "xre_readline.h"
#include "xre_runtime.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static void history_free_entry(void *entry)
{
	free(*(void **)entry);
}

static bool history_init(history_array_t **history)
{
	*history = array_create(sizeof(char *), 10, history_free_entry);
	return (*history);
}

static void history_fini(history_array_t *history)
{
	array_kill(history);
}

static bool history_entry_push(history_array_t *history, void *entry)
{
	return (array_pushf(history, &entry));
}

static bool block_process(const char *block)
{
	if (!block || !*block)
		return (true);

	xre_ast_t *ast = xre_ast_compose(block);
	if (ast) {
		(void)xre_runtime(ast);

		ast_free(ast);
		return (true);
	}

	return (false);
}

bool xre_repl_entrypoint(void)
{
	history_array_t *history;
	void *line_buffer = NULL;

	if (!history_init(&history)) {
		return (false);
	}

	(void)xre_repl_sigset_default();

	for (;;) {
		if (!xre_read_block(&line_buffer, history)) {
			goto prison;
		}

		(void)block_process(line_buffer);

		if (!history_entry_push(history, line_buffer)) {
			free(line_buffer);
			goto prison;
		}
	}

	history_fini(history);
	xre_repl_clear_signals();

	return (true);

prison:
	history_fini(history);
	xre_repl_clear_signals();

	return (false);
}
