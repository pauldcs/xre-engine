#include "xre_nodes.h"
#include "xre_runtime.h"
#include "defaults.h"

const struct builtin builtin_lookup[] = {
	{
		.iden = "std_map",
		.type = EXPR_OP_TYPE_BINOP,
		.meta = { .kind	       = BUILTIN_CALL,
			  .type_rule   = RETURN_TYPE_RULE_LEFT,
			  .offset_rule = RETURN_OFFSET_RULE_YIELD,
			  .profile = { .ret  = { .protection = RDWR,
						 .type = VEC_OBJECT },
				       .args = { .count = 2,
						 .ports = { __undefined_port,
							    __undefined_port } } } },
	},
	{
		.iden = "std_out",
		.type = EXPR_OP_TYPE_UNIOP,
		.meta = { .kind	       = BUILTIN_CALL,
			  .type_rule   = RETURN_TYPE_RULE_LEFT,
			  .offset_rule = RETURN_OFFSET_RULE_LEFT,
			  .profile     = { .ret	 = __undefined_port,
					   .args = { .count = 1,
						     .ports = { __undefined_port } } } },
	},
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
