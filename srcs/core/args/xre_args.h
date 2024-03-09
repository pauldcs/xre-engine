#ifndef __XRE_ARGS_H__
# define __XRE_ARGS_H__

# include <stddef.h>
# include <stdbool.h>
# include <sys/types.h>
# include <stdint.h>

# define FILE_STACK_SIZE 16

extern void  *__stack[FILE_STACK_SIZE];
extern size_t __top;

typedef struct {
	bool          fail;
	void          *arg;
	size_t        ac;
	unsigned char **av;
	const char 	  *ostr;
} t_xre_getopts;

typedef enum {
    FLAGS_DEBUG      = 1 << 1,
    SHOW_ERRORS      = 1 << 2,
		SHOW_EXPR_RESULT = 1 << 3,
}   t_flags;

typedef struct {
	t_flags   flags;
	uint32_t  argument_a;
	char 	   *code;
}	t_xre_args;

extern t_xre_args   __xre_args__;

t_xre_args *xre_args_parse(int ac, char *av[]);
void       *filename_stack_pop(void);

#endif /* __XRE_ARGS_H__ */
