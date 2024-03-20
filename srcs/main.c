#include "xre_alloc.h"
#include "xre_args.h"
#include "xre_assert.h"
#include "xre_core.h"
#include "xre_fs.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_readline.h"
#include "xre_repl.h"
#include "xre_runtime.h"
#include <string.h>
#include <sys/stat.h>

t_xre_state __xre_state__ = {
	.title = "xre",
	.version = "0.5.6",
};

static bool init_source_file(t_xre_args *args, const char *path)
{
	struct stat sb;
	int fd;
	char *buffer = NULL;

	(void)args;
	if (stat(path, &sb) == -1) {
		xre_report_error("%s: %s", path, strerror(errno));
		return (false);
	}

	switch (sb.st_mode & S_IFMT) {
	case S_IFCHR:
		/* handle character device */
	case S_IFBLK:
		/* handle block device */
	case S_IFIFO:
		/* handle pipe */
	case S_IFLNK:
		/* handle symlink */
	case S_IFSOCK:
		/* handle socket */
		xre_report_error("%s: Invalid file type", path);
		break;
	case S_IFDIR:
		/* handle directory */
		xre_report_error("%s: %s", path, "Is a directory");
		break;

	case S_IFREG:
		/* handle regular file */
		if ((sb.st_mode & ALLPERMS) & (S_IRUSR | S_IRGRP | S_IROTH)) {
			if (!xre_file_open_read(path, &fd))
				return (false);

			goto club;
		}
		xre_report_error("%s: %s", path, "Permission denied");
		return (false);

	default:
		xre_report_error("%s: %s", path, "Foreigh tongue");
		return (false);
	}
	return (false);

	__assert_not_reached__();

club:

	buffer = xmalloc(sb.st_size + sizeof(char));
	if (!buffer)
		return (false);

	(void)memset(buffer, 0, sb.st_size + sizeof(char));

	if (read(fd, buffer, sb.st_size + sizeof(char)) == -1) {
		goto prison;
	}

	xre_ast_t *ast = xre_ast_compose(buffer);
	if (!ast) {
		goto prison;
	}

	if (!xre_runtime(ast)) {
		ast_free(ast);
		goto prison;
	}

	free(buffer);
	ast_free(ast);
	return (true);

prison:
	free(buffer);
	return (false);
}

int main(int ac, char *av[])
{
	t_xre_args *args;
	xre_ast_t *ast;
	char *file;

	if (!(args = xre_args_parse(ac, av)))
		return (EXIT_FAILURE);

	__xre_args__ = *args;

	for (;;) {
		if (__top) {
			file = filename_stack_pop();
			if (file) {
				if (!init_source_file(args, file)) {
					goto prison;
				}
			} else {
				fprintf(stderr,
					"%s: Failed to import input file\n",
					__xre_state__.title);
				goto prison;
			}
		} else {
			if (args->code) {
				ast = xre_ast_compose(args->code);
				if (!ast) {
					goto prison;
				}

				if (!xre_runtime(ast)) {
					ast_free(ast);
					goto prison;
				}

				ast_free(ast);
				free(args);
				return (true);

			} else {
				if (!xre_repl_entrypoint()) {
					goto prison;
				}
			}
		}

		if (!__top)
			break;
	}

prison:
	free(args);
	return (false);
}
