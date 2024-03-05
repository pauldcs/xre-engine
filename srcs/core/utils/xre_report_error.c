#include "xre_core.h"
#include <stdarg.h>
#include <stdio.h>

void
xre_report_error(const char* format, ...) {

    fprintf(stderr, "%s: ", __xre_state__.title);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
