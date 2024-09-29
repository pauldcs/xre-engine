#ifndef __XRE_ARGS_H__
#define __XRE_ARGS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define FILE_STACK_SIZE 16

extern void  *__stack[FILE_STACK_SIZE];
extern size_t __top;

typedef struct {
	bool		fail;
	void	       *arg;
	size_t		ac;
	unsigned char **av;
	const char     *ostr;
} t_xre_getopts;

typedef enum {
	FLAGS_DEBUG	    = 1 << 1,
	FLAGS_DEBUG_VERBOSE = 1 << 2,
} t_flags;

typedef struct {
	t_flags	 flags;
	uint32_t argument_a;
	char	*code;
} t_xre_args;

extern t_xre_args __xre_args__;

t_xre_args *xre_args_parse(int ac, char *av[]);
void	   *filename_stack_pop(void);

#endif /* __XRE_ARGS_H__ */
