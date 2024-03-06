#include "xre_args.h"
#include "xre_repl.h"
#include "xre_log.h"
#include "xre_core.h"
#include "xre_alloc.h"
#include "xre_fs.h"
#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_readline.h"
#include "xre_log.h"
#include "xre_parse.h"
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

t_xre_state  __xre_state__ = {
	.title = "xre",
	.version = "0.5",
};

// void free_ast(t_xre_ast *ast) {
// 	if (ast->kind)
// 	free_ast()
// }

static bool
init_source_file(t_xre_args *args, const char *path) {	

	struct stat  sb;
	int          fd;
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
			if ((sb.st_mode & ALLPERMS)
				& (S_IRUSR
					| S_IRGRP
					| S_IROTH)
			) {
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
		free(buffer);
		return (false);
	}

	t_xre_ast *ast = xre_ast_compose(buffer);
	if (!xre_runtime(ast)) {
		free(buffer);
		return (false);
	}

	free(buffer);
 	return (true);
}

int
main(int ac, char *av[]) {

	t_xre_args *args;
	char *file;

	if (!(args = xre_args_parse(ac, av)))
		return (EXIT_FAILURE);

	__xre_args__ = *args;


	for (;;) {
		if (__top) {
			file = filename_stack_pop();
			if (file) {
				if (!init_source_file(args, file))
					return (free(args), false);
			} else {
				fprintf(stderr, "%s: Failed to import '%s'\n",
					__xre_state__.title,
					file
				);
				xfree(args);
				return (free(args), false);
			}
		} else {
	
			if (!xre_repl_entrypoint()) {
				return (free(args), false);
			}
		}

		if (!__top)
			break;
	}

	free(args);
	return (0);
}