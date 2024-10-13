#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#define __number_port                   \
	{                               \
		.protection = RD, .type = I64 \
	}

#define __undefined_port                      \
	{                                     \
		.protection = WR, .type = UNDEFINED \
	}

#define __readable_undefined_port               \
	{                                       \
		.protection = RDWR, .type = UNDEFINED \
	}

#define __readable_string_port             \
	{                                  \
		.protection = RD, .type = STRING \
	}

#define __readable_sequence_port               \
	{                                      \
		.protection = RD, .type = VEC_OBJECT \
	}

#define __arithmetic_op_profile(__op)                             \
	{                                                         \
		.kind = __op, .type_rule = RETURN_TYPE_RULE_INHERIT, \
		.offset_rule = RETURN_OFFSET_RULE_YIELD, .profile = {  \
			.ret  = __number_port,                    \
			.args = { .count = 2,                     \
				  .ports = { __number_port,       \
					     __number_port } }    \
		}                                                 \
	}

#define __logic_op_profile(__op)                                   \
	{                                                          \
		.kind = __op, .type_rule = RETURN_TYPE_RULE_INHERIT,  \
		.offset_rule = RETURN_OFFSET_RULE_INHERIT, .profile = { \
			.ret  = __undefined_port,                  \
			.args = { .count = 2,                      \
				  .ports = { __number_port,        \
					     __undefined_port } }  \
		}                                                  \
	}

#define __do_op_profile                                            \
	{                                                          \
		.kind = LOGIC_DO, .type_rule = RETURN_TYPE_RULE_LEFT, \
		.offset_rule = RETURN_OFFSET_RULE_LEFT, .profile = {    \
			.ret  = __number_port,                     \
			.args = { .count = 2,                      \
				  .ports = { __number_port,        \
					     __undefined_port } }  \
		}                                                  \
	}

#define __compare_op_profile(__op)                                \
	{                                                         \
		.kind = __op, .type_rule = RETURN_TYPE_RULE_INHERIT, \
		.offset_rule = RETURN_OFFSET_RULE_YIELD, .profile = {  \
			.ret  = __number_port,                    \
			.args = { .count = 2,                     \
				  .ports = { __undefined_port,    \
					     __undefined_port } } \
		}                                                 \
	}

#endif /* __DEFAULT_H__ */
