#include "xre_args.h"
#include "xre_errors.h"
#include "xre_nodes.h"
#include "xre_stringf.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int __fdout__ = STDERR_FILENO;

void xre_error(err_notif_t *notification)
{
	static char err_buff[4096];
	size_t	    i = 0;

	const struct token *token = notification->orig;

	if (!(__xre_args__.flags & SHOW_ERRORS)) {
		i += cpyf(&err_buff[i], 4096 - i, "Exception: ");
		i +=
			cpyf(&err_buff[i],
			     4096 - i,
			     "%s: ",
			     error_class_str(notification->class));
		i +=
			cpyf(&err_buff[i],
			     token->_line_len,
			     "%s",
			     token->_line_ptr);
		i +=
			cpyf(&err_buff[i],
			     4096 - i,
			     ": %s\n",
			     error_type_str(notification->type));
		(void)write(STDERR_FILENO, err_buff, i);
		return;
	}
	// (void)fputstr(__fdout__,
	//   "\"xre_error\": {\n"
	//   "  \"line\": %d\n"
	//   "  \"column\": %d\n"
	//   "  \"token\": \"%s\"\n"
	//   "  \"errtype\": \"%s\"\n"
	//   "  \"errsubtype\": \"%s\"\n"
	//   "}\n",
	//     token->_line,
	//     token->_cols,
	//     expr_kind_to_string(token->_kind),
	//     err_type_to_string(report->error.type),
	//     err_subtype_to_string(report->error.subtype)
	// );

	(void)memset(err_buff, 0x00, sizeof(err_buff));

	i +=
		cpyf(err_buff,
		     4096,
		     "Exception [%s]",
		     error_class_str(notification->class));

	i +=
		cpyf(&err_buff[i],
		     4096 - i,
		     "\n at %s line: %d, column: %d.\n  |",
		     expr_kind_to_string(token->_kind),
		     token->_line,
		     token->_cols);
	i += cpyf(&err_buff[i], 4096 - i, "\n  |  ");
	i += cpyf(
		&err_buff[i], token->_line_len, "%s", token->_line_ptr
	);
	i += cpyf(&err_buff[i], 4096 - i, " \n  |  ");

	size_t size = token->_cols;
	while (size--) {
		i += cpyf(&err_buff[i], 4096 - i, " ");
	}

	i += cpyf(&err_buff[i], 4096 - i, "\033[1;31m");
	i += cpyf(&err_buff[i], 4096 - i, "^(");
	i +=
		cpyf(&err_buff[i],
		     4096 - i,
		     "%s) \033[0m",
		     error_type_str(notification->type));
	i += cpyf(&err_buff[i], 4096 - i, "\n  |\n");

	(void)write(STDERR_FILENO, err_buff, i);
}
