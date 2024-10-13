#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char *format_string(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	int len = vsnprintf(NULL, 0, format, args);
	va_end(args);

	if (len < 0)
		return (NULL);

	char *result = (char *)malloc(len + 1);
	if (!result)
		return (NULL);

	va_start(args, format);
	vsnprintf(result, len + 1, format, args);
	va_end(args);

	return (result);
}
