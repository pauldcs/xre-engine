#include "xre_alloc.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/* Read FILENAME into a locally-allocated buffer and return the buffer.
   The size of the buffer is returned in *SIZEP.  Returns NULL if any
   errors were encountered. */
char *read_file(char *filename, size_t *sizep)
{
	struct stat finfo;
	size_t file_size;
	char *buffer;
	int i, file;

	if ((stat(filename, &finfo) < 0) ||
	    (file = open(filename, O_RDONLY, 0666)) < 0)
		return (NULL);

	file_size = (size_t)finfo.st_size;

	/* check for overflow on very large files */
	if (file_size != finfo.st_size || file_size + 1 < file_size) {
		if (file >= 0)
			close(file);
#if defined(EFBIG)
		errno = EFBIG;
#endif
		return (NULL);
	}

	/* Read the file into BUFFER. */
	buffer = xmalloc(file_size + 1);
	i = read(file, buffer, file_size);
	close(file);

	if (i < 0) {
		free(buffer);
		return (NULL);
	}

	buffer[i] = '\0';
	if (sizep)
		*sizep = i;

	return (buffer);
}
