#include "xre_errors.h"
#include "xre_parse.h"
#include "xre_args.h"
#include "xre_stringf.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int __fdout__ = STDERR_FILENO;

void xre_error(t_xre_error *report, const xre_token_t *token) {
  static char err_buff[4096];
  size_t i = 0;

  if (!(__xre_args__.flags & SHOW_ERRORS)) {
    i += cpyf(&err_buff[i], 4096 - i, "error: ");
    i += cpyf(&err_buff[i], 4096 - i, "%s: ", err_type_to_string(report->error.type));
    i += cpyf(&err_buff[i], token->_line_len, "%s", token->_line_ptr);
    i += cpyf(&err_buff[i], 4096 - i, ": %s\n", err_subtype_to_string(report->error.subtype));
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

  i += cpyf(err_buff, 4096, "error: %s%s%s", "\033[1m",
            err_type_to_string(report->error.type), "\033[0m");

  i += cpyf(&err_buff[i], 4096 - i, " at line: %d, column: %d.\n|", token->_line,
            token->_cols);
  i += cpyf(&err_buff[i], 4096 - i, "\n|    ");
  // i += cpyf(&err_buff[i], 4096 - i, "\033[44m");
  i += cpyf(&err_buff[i], token->_line_len, "%s", token->_line_ptr);
  // i += cpyf(&err_buff[i], 4096 - i, "\033[0m");
  i += cpyf(&err_buff[i], 4096 - i, " \n|    ");

  size_t size = token->_cols;
  while (size--) {
    i += cpyf(&err_buff[i], 4096 - i, " ");
  }

  i += cpyf(&err_buff[i], 4096 - i, "\033[1;31m");
  i += cpyf(&err_buff[i], 4096 - i, "^--");
  i += cpyf(&err_buff[i], 4096 - i, " %s: \033[0m%s",
            expr_kind_to_string(token->_kind),
            err_subtype_to_string(report->error.subtype));

  i += cpyf(&err_buff[i], 4096 - i, "\n|\n");

  (void)write(STDERR_FILENO, err_buff, i);
}
