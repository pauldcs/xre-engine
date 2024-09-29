#include "xre_nodes.h"
#include "xre_builtin.h"
#include "xre_runtime.h"

const struct builtin builtin_lookup[] = {
	{ "std_map",
	  EXPR_OP_TYPE_BINOP,
	  O_TYPE_SEQUENCE | O_ATTR_READABLE | O_ATTR_MUTABLE,
	  O_TYPE_SEQUENCE | O_TYPE_UNDEFINED,
	  O_TYPE_UNDEFINED },

	{ "std_out",
	  EXPR_OP_TYPE_UNIOP,
	  O_TYPE_UNDEFINED,
	  O_ATTR_READABLE,
	  O_TYPE_UNDEFINED },
	{ "std_buf",
	  EXPR_OP_TYPE_UNIOP,
	  O_TYPE_BUFFER | O_ATTR_READABLE | O_ATTR_MUTABLE,
	  O_ATTR_READABLE,
	  O_TYPE_UNDEFINED }
};

struct builtin *builtin_find(const char *lookup_name, size_t size)
{
	size_t i = 0;
	size_t lookup_size =
		sizeof(builtin_lookup) / sizeof(builtin_lookup[0]);

	while (i < lookup_size) {
		const char *name = builtin_lookup[i].iden;
		if (strlen(name) == size &&
		    !memcmp(name, lookup_name, size)) {
			return ((struct builtin *)&builtin_lookup[i]);
		}
		i++;
	}

	return (NULL);
}
