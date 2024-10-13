#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

static bool run_python_script(void *code)
{
	static struct {
		char *_av[4];
		char *_ep[1];
	} args = { ._av = { "/usr/bin/python3",
			    "/root/pp/preprocessor.py",
			    NULL,
			    NULL },
		   ._ep = { NULL } };

	args._av[2] = code;

	switch (fork()) {
	case -1:
		goto hell;

	case 0:
		(void
		)execve(*args._av,
			(char *const *)args._av,
			(char *const *)args._ep);
		exit(EXIT_FAILURE);

	default:

		while (waitpid(0, NULL, 0) != -1)
			;
		return (true);
	}

hell:
	return (false);
}

/*
 *    Used by `copy_from_user_stdin`, reads one 
 *    character from stdin and returns 0 if it's a newline
 */
static inline int read_one_byte_from_stdin(uint8_t *dest)
{
	ssize_t ret = 0;
retry:
	ret = read(STDIN_FILENO, dest, sizeof(uint8_t));
	if (ret == -1) {
		if (errno != EAGAIN) {
			return (-1);
		}

		goto retry;
	}
	if (ret == 0) {
		*dest = '\0';
		return (0);
	}

	switch (*dest) {
	case 10:
		*dest = '\0';
		return (0);
	}

	return (1);
}

static void copy_from_user_stdin(uint8_t *buffer)
{
	while (read_one_byte_from_stdin(buffer++) == 1) {
		;
	}
}

static void copy_from_user_argv(uint8_t *buffer, const char *data)
{
	static struct stat finfo;
	static int	   fd;

	if (access(data, R_OK) != 0) {
		while (*data) {
			*buffer++ = *data++;
		}

		*buffer = '\0';

	} else {
		if ((stat(data, &finfo) == -1) ||
		    (fd = open(data, O_RDONLY, 0666)) == -1) {
			exit(EXIT_FAILURE);
		}

		if (read(fd, buffer, (size_t)finfo.st_size) != -1) {
			buffer[finfo.st_size] = '\0';
		}

		(void)close(fd);
	}
}

bool preprocess(int ac, char **av)
{
	uint8_t buffer[4096];
	(void)memset(&buffer, 0, sizeof(buffer));

	if (ac < 2) {
		copy_from_user_stdin((uint8_t *)&buffer);
	} else {
		copy_from_user_argv((uint8_t *)&buffer, av[1]);
	}
	run_python_script(buffer);
	return (true);
}

int main(int ac, char **av)
{
	return (!preprocess(ac, av));
}
