#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

bool timespec_to_string(
	const struct timespec st_mtimespec,
	char		     *dest,
	const char	     *format,
	size_t		      dest_size
)
{
	assert(dest != NULL);
	assert(dest_size != 0);

	time_t	  time_val = st_mtimespec.tv_sec;
	struct tm time_struct;

	if (localtime_r(&time_val, &time_struct) == NULL) {
		return (false);
	}

	if (strftime(
		    dest,
		    dest_size,
		    format,
		    /* "%Y-%m-%d %H:%M:%S" */ &time_struct
	    ) == 0) {
		return (false);
	}
	return (true);
}
