#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

bool st_mode_to_string(mode_t st_mode, char *dest, size_t dest_size)
{
	assert(dest != NULL);
	assert(dest_size != 0);

	int result = snprintf(dest, dest_size, "%c%c%c%c%c%c%c%c%c",
			      (st_mode & S_IRUSR) ? 'r' : '-',
			      (st_mode & S_IWUSR) ? 'w' : '-',
			      (st_mode & S_IXUSR) ? 'x' : '-',
			      (st_mode & S_IRGRP) ? 'r' : '-',
			      (st_mode & S_IWGRP) ? 'w' : '-',
			      (st_mode & S_IXGRP) ? 'x' : '-',
			      (st_mode & S_IROTH) ? 'r' : '-',
			      (st_mode & S_IWOTH) ? 'w' : '-',
			      (st_mode & S_IXOTH) ? 'x' : '-');

	if (result < 0 || (size_t)result >= dest_size) {
		return (false);
	}
	return (true);
}
