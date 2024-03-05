#include "xre_core.h"
#include "xre_log.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

t_log_level log_level = debug;

void xre_report_error(const char* format, ...)
{
    fprintf(stderr, "%s: ", __xre_state__.title);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

static inline void get_current_time(char* buffer, size_t buffer_size)
{
    struct timeval tv;
	struct tm tm_time;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm_time);
    strftime(buffer, buffer_size, " %H:%M:%S", &tm_time);
    snprintf(buffer + 19, buffer_size - 19, ".%03d", (int)tv.tv_usec / 1000);
}

static const char* level_tags[] = {
	[verbose]  = "verbose",
    [debug]    = "debug",
    [info]     = "info",
    [warning]  = "warning",
    [error]    = "error",
    [fatal]    = "fatal"
};

static const char* level_colors[] = {
	[verbose]  = "\x1b[90m",
    [debug]    = "\033[0;33m",
    [info]     = "\033[0;34m",
    [warning]  = "\033[0;35m",
    [error]   = "\033[0;31m",
    [fatal]   = "\033[1;31m"
};

/**
 * @brief Prints a log into stderr.
 * 
 * The function logs the string only if the given level
 * is equal or above the global `log_level`.
 * Note: the `strerror(errno)` macros is available and equals
 * `strerror(errno)`
 * 
 * @param level debug | info | warning | error | fatal 
 * @param format format string
 * @param ... 
 */

void xre_logger(t_log_level level, const char *filename, const char *func, uint32_t lineno, const char* format, ...)
{
	if (level < log_level)
        return;

    char ts[24];
    get_current_time(ts, sizeof(ts));

    const char* level_string = level_tags[level];
    const char* level_color = level_colors[level];

    if (level == info) {
        fprintf(stderr,
            "%s [%s%s%s]: ",
            ts,
            level_color,
            level_string,
            "\033[0m"
        );
    } else {
        fprintf(stderr,
            "%s [%s%s%s] (%s: %s: %u): ",
            ts,
            level_color,
            level_string,
            "\033[0m",
            filename,
            func,
            lineno
        );
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}

void xre_log_set_level(t_log_level level) {
	log_level = level;
}
