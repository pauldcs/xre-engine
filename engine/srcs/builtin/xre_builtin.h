#ifndef __XRE_BUILTIN_H__
#define __XRE_BUILTIN_H__

#include "xre_nodes.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct builtin {
	const char    *iden;
	enum expr_type type;

	int64_t self_attr;
	int64_t left_attr;
	int64_t right_attr;
};

extern const struct builtin builtin_lookup[3];

struct builtin *builtin_find(const char *lookup_name, size_t size);

#endif /* __XRE_BUILTIN_H__ */
