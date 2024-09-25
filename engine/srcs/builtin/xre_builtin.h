#ifndef __XRE_BUILTIN_H__
#define __XRE_BUILTIN_H__

#include <stdbool.h>
#include <stddef.h>

enum builtin_type {
	BUILTIN_BINOP,
	BUILTIN_UNIOP,
};

struct builtin {
	const char	 *iden;
	enum builtin_type type;
	void		 *func;
};

extern struct builtin builtin_lookup[8];

bool		  is_defined_builtin(const char *ptr, size_t size);
const char	 *builtin_get_name(const char *ptr, size_t size);
enum builtin_type builtin_get_type(const char *ptr, size_t size);

#endif /* __XRE_BUILTIN_H__ */
