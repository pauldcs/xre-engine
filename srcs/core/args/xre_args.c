#include "xre_args.h"
#include "xre_alloc.h"
#include "xre_utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

t_xre_args __xre_args__;

void __attribute__((noreturn)) usage(void)
{
	(void)fprintf(stderr,
		      "XRE Interpreter v%s\n"
		      "usage: %s [-resdh] [-x [CODE]] [INFILES] ...\n\n"
		      "options:\n"
		      "    -x   [CODE]   Execute code from command line\n"
		      "    -r            Print command results\n"
		      "    -e            Show good error messages\n"
		      "    -d            Enable ast debug mode\n"
		      "    -h            Show this help message\n\n",

		      __xre_state__.version, __xre_state__.title);
	exit(0);
}

/*
 *    mini stack to hold each input file sepraratly
 */
void *__stack[FILE_STACK_SIZE];
size_t __top;

/*
 *    Only purpose is to reverse the filename
 *    stack so that the files are consumed in the
 *    same order as they were given in arguments
 */
void stack_reverse_in_place(void)
{
	size_t i = 1;
	size_t j = __top;
	void *tmp;

	while (i < j) {
		tmp = __stack[i];
		__stack[i] = __stack[j];
		__stack[j] = tmp;
		i++;
		j--;
	}
}

void filename_stack_push(void *filename)
{
	if (__top < FILE_STACK_SIZE - 1)
		__stack[++__top] = filename;
}

void *filename_stack_pop(void)
{
	if (__top > 0)
		return ((void *)__stack[__top--]);
	return (NULL);
}

static unsigned char *next_arg(size_t *ac, unsigned char ***av)
{
	if (*ac) {
		--(*ac);
		return ((unsigned char *)*++*av);
	}
	return (NULL);
}

static void xre_getopts_init(t_xre_getopts *opt, size_t ac, const char *av[],
			     const char *ostr)
{
	memset(opt, 0, sizeof(t_xre_getopts));
	opt->ac = --ac;
	opt->av = (unsigned char **)++av;
	opt->ostr = ostr;
}

/*
 * Todo:
 *    implement `-a -b -c == -abc`
 *         maybe push the chained options to a stack,
 *         + the function should not be allowed to take additional arguments if
 * the stack is not empty (only 'c' could take an additional argument)
 */
static int xre_getopts_next(t_xre_getopts *opts)
{
	int ret = -1;
	if (!opts->ac || !opts->av)
		return (-1);

	opts->arg = NULL;
	unsigned char *ptr = (unsigned char *)*opts->av;

	while (isspace(*ptr))
		++ptr;

	if (*ptr == '-' && *(ptr + 1) && !*(ptr + 2)) {
		if (*(ptr + 1) == '-')
			return (-1);

		char *ch = strchr(opts->ostr, *(ptr + 1));
		if (!ch) {
			fprintf(stderr, "illegal option -- %c\n", *(ptr + 1));
			++opts->av;
			--opts->ac;
			return ((int)'?');
		}

		ret = *ch;
		if (*(ch + 1) == ':') {
			opts->arg = next_arg(&opts->ac, &opts->av);
			if (!opts->arg) {
				fprintf(stderr,
					"option requires an argument -- %c\n",
					ret);
				opts->fail = true;
				return (-1);
			}
		}
		++opts->av;

	} else {
		opts->arg = ptr;
		++opts->av;
		ret = ((int)'*');
	}

	--opts->ac;
	return (ret);
}

static bool uint32_parse(const char *expr, uint32_t *dest)
{
	const char *ret = str_to_uint32(expr, dest);
	if (ret == NULL || *ret != 0) {
		fprintf(stderr, "%s: Is not an integer", expr);
		return (false);
	}
	return (true);
}

static bool string_parse(const char *expr, char **dest)
{
	if (!expr) {
		fprintf(stderr, "Missing string");
		return (false);
	}
	*dest = (char *)expr;
	return (true);
}

t_xre_args *xre_args_parse(int ac, char *av[])
{
	t_xre_args *args = xmalloc(sizeof(t_xre_args));
	t_xre_getopts xopts;
	char c;

	if (!args)
		return (NULL);

	bzero(args, sizeof(t_xre_args));
	xre_getopts_init(&xopts, ac, (const char **)av, "drhex:");

	while ((c = xre_getopts_next(&xopts)) != (char)-1) {
		switch (c) {
		case 'd':
			args->flags |= FLAGS_DEBUG;
			break;
		case 'e':
			args->flags |= SHOW_ERRORS;
			break;
		case 'r':
			args->flags |= SHOW_EXPR_RESULT;
			break;
		case 'v':
			if (!uint32_parse(xopts.arg, &args->argument_a))
				return (free(args), NULL);

			break;
		case 'x':
			if (!string_parse(xopts.arg, &args->code))
				return (free(args), NULL);

			break;
		case '*':
			/*
       *     if it does not match any of the
       *     options it's a filename unless it
       *     is '-' or 'stdin'
       */
			if (__top == FILE_STACK_SIZE)
				break;
			if (xopts.arg == NULL || !strcmp(xopts.arg, "-") ||
			    !strcmp(xopts.arg, "stdin"))
				filename_stack_push("/dev/stdin");
			else
				filename_stack_push(xopts.arg);

			break;
		case 'h':
		case '?':
			usage();
		}
	}
	if (xopts.fail) {
		return (free(args), NULL);
	}
	/*
   *     the argument parsing can stop at '--' so
   *     if there are remaining arguments it's
   *     filnames
   */
	while (*xopts.av++ && *xopts.av)
		filename_stack_push((char *)*xopts.av);

	stack_reverse_in_place();
	return (args);
}
