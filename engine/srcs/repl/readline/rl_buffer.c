#include "vec.h"
#include "xre_readline.h"
#include <stdbool.h>
#include <string.h>

bool rl_buf_add_nullterm(void)
{
	return (vec_inject(__state__._v, __state__._ei, "\0", 1));
}

bool rl_buf_inject_at(size_t at, void *data, size_t size)
{
	bool success = vec_inject(__state__._v, at, data, size);
	if (success) {
		__state__._i += size;
		__state__._ei += size;
		if (__state__._ei > __state__._mei)
			++__state__._mei;
	}
	return (success);
}

void rl_buf_delete_at(size_t at, size_t size)
{
	vec_wipe(__state__._v, at - size, at);
	__state__._ei -= size;
	__state__._i -= size;
}

void rl_discard_line(void)
{
	vec_kill(__state__._v);
	(void)memset(&__state__, 0x00, sizeof(__state__));
}

void rl_replace_buffer(const char *string)
{
	if (string) {
		vec_clear(__state__._v);
		(void
		)vec_append(__state__._v, string, strlen(string));
		__state__._i   = 0;
		__state__._ei  = strlen(string);
		__state__._mei = strlen(string);
	}
}
