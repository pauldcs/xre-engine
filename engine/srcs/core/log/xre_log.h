#ifndef ____xre_loggerGER_H__
#define ____xre_loggerGER_H__

#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include <stdint.h>

#if __MINGW32__ || (defined(_WIN32) || defined(_WIN64)) && !defined(__GNUC__)
#define MACRO_LOG_FUNC __FUNCTION__
#define MACRO_WEAK_SYM
#elif defined(__EMSCRIPTEN__)
#define MACRO_LOG_FUNC __func__
#define MACRO_WEAK_SYM
#else
#define MACRO_LOG_FUNC __func__
#define MACRO_WEAK_SYM __attribute__((weak))
#endif

#define __xre_logger(lvl, fmtstr, ...)                              \
	xre_logger(lvl, __FILE__, MACRO_LOG_FUNC, __LINE__, fmtstr, \
		   ##__VA_ARGS__);

typedef enum e_log_level {
	verbose,
	info,
	debug,
	warning,
	error,
	fatal
} t_log_level;

extern t_log_level __log_level__;

void xre_logger(t_log_level level, const char *filename, const char *func,
		uint32_t lineno, const char *format, ...);
void xre_report_error(const char *format, ...);

#endif /* ____xre_logger_H__ */
