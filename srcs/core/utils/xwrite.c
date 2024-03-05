#include "xre_core.h"
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

ssize_t xwrite(int fd, const unsigned char *buf, size_t nbytes)
{
	int i;
	int ntry = 0;

	size_t limit = 0x7FFFFFFF;
    size_t n = nbytes < limit ? nbytes : limit;

	for (;;)
	{
		i = write(fd, buf, n);
		if (i > 0)
		{
			if ((n -= i) <= 0)
				return (nbytes);
			buf += i;
			ntry = 0;
	
		} else if (i == 0)
		{
			if (++ntry > 10)
				return (nbytes - n);
	
		} else if (errno != EINTR)
			return (-1);
	}
}
