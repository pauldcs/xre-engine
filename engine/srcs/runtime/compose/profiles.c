#include "xre_runtime.h"
#include "defaults.h"

struct expression_meta exp_meta_lookup[] = {
	__arithmetic_op_profile(ARITHMETHIC_ADD),
	__arithmetic_op_profile(ARITHMETHIC_SUB),
	__arithmetic_op_profile(ARITHMETHIC_MUL),
	__arithmetic_op_profile(ARITHMETHIC_MOD),
	__arithmetic_op_profile(ARITHMETHIC_DIV),
	__arithmetic_op_profile(ARITHMETHIC_POW),
	__arithmetic_op_profile(BIT_AND),
	__arithmetic_op_profile(BIT_OR),
	__arithmetic_op_profile(BIT_XOR),
	__arithmetic_op_profile(BIT_RSHIFT),
	__arithmetic_op_profile(BIT_LSHIFT),
	__compare_op_profile(COMPARE_LT),
	__compare_op_profile(COMPARE_GT),
	__compare_op_profile(COMPARE_LE),
	__compare_op_profile(COMPARE_GE),
	__compare_op_profile(COMPARE_EQ),
	__compare_op_profile(COMPARE_NE),
	__logic_op_profile(LOGIC_AND),
	__logic_op_profile(LOGIC_OR),
	__logic_op_profile(LOGIC_ELSE),
	{ .kind	   = LOGIC_DO,
	  .t_rule  = RETURN_TYPE_RULE_LEFT,
	  .o_rule  = RETURN_OFFSET_RULE_LEFT,
	  .profile = { .ret  = __number_port,
		       .args = { .count = 2,
				 .ports = { __number_port,
					    __undefined_port } } } },
	{ .kind	   = LOGIC_LOOP,
	  .t_rule  = RETURN_TYPE_RULE_LEFT,
	  .o_rule  = RETURN_OFFSET_RULE_LEFT,
	  .profile = { .ret  = __number_port,
		       .args = { .count = 2,
				 .ports = { __number_port,
					    __undefined_port } } } },
	{ .kind	   = LOGIC_NOT,
	  .t_rule  = RETURN_TYPE_RULE_LEFT,
	  .o_rule  = RETURN_OFFSET_RULE_YIELD,
	  .profile = { .ret  = __number_port,
		       .args = { .count = 1,
				 .ports = { __number_port } } } },
	{ .kind	   = CLOSURE,
	  .t_rule  = RETURN_TYPE_RULE_RIGHT,
	  .o_rule  = RETURN_OFFSET_RULE_RIGHT,
	  .profile = { .ret  = __number_port,
		       .args = { .count = 2,
				 .ports = { __readable_undefined_port,
					    __undefined_port } } } },
	{ .kind	   = ASSIGN,
	  .t_rule  = RETURN_TYPE_RULE_LEFT,
	  .o_rule  = RETURN_OFFSET_RULE_LEFT,
	  .profile = { .ret  = __undefined_port,
		       .args = { .count = 2,
				 .ports = { __readable_undefined_port,
					    __undefined_port } } } },
	{ .kind	   = SEPARATOR,
	  .t_rule  = RETURN_TYPE_RULE_RIGHT,
	  .o_rule  = RETURN_OFFSET_RULE_RIGHT,
	  .profile = { .ret  = __undefined_port,
		       .args = { .count = 2,
				 .ports = { __undefined_port,
					    __undefined_port } } } },
	{ .kind	   = SEQUENCE,
	  .t_rule  = RETURN_TYPE_RULE_RIGHT,
	  .o_rule  = RETURN_OFFSET_RULE_RIGHT,
	  .profile = { .ret  = __undefined_port,
		       .args = { .count = 2,
				 .ports = { __undefined_port,
					    __undefined_port } } } }
};

struct expression_meta *expr_profile_lookup(enum expression_kind kind)
{
	for (size_t i = 0;
	     i < sizeof(exp_meta_lookup) / sizeof(exp_meta_lookup[0]);
	     ++i) {
		if (exp_meta_lookup[i].kind == kind) {
			return ((struct expression_meta
					 *)&exp_meta_lookup[i]);
		}
	}
	return (NULL);
}
