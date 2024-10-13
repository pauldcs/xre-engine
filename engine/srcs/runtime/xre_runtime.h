#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_errors.h"
#include "xre_args.h"
#include "xre_nodes.h"
#include "dstr.h"
#include <stdbool.h>

enum expression_kind {
	ARITHMETHIC_ADD,
	ARITHMETHIC_SUB,
	ARITHMETHIC_MUL,
	ARITHMETHIC_MOD,
	ARITHMETHIC_DIV,
	ARITHMETHIC_POW,
	LOGIC_AND,
	LOGIC_OR,
	LOGIC_DO,
	LOGIC_ELSE,
	LOGIC_LOOP,
	LOGIC_NOT,
	BIT_AND,
	BIT_OR,
	BIT_XOR,
	BIT_LSHIFT,
	BIT_RSHIFT,
	COMPARE_LT,
	COMPARE_GT,
	COMPARE_LE,
	COMPARE_GE,
	COMPARE_EQ,
	COMPARE_NE,
	REFERENCE,
	SEQUENCE,
	CLOSURE,
	ASSIGN,
	SEPARATOR,
	BUILTIN_CALL,
	IMPOSSIBLE,
};

enum expression_kind expression_get_new_kind(enum expr_kind kind);
const char *expression_kind_string(enum expression_kind kind);

enum memory_prot { PROT_NONE, RD, WR, RDWR };

enum memory_type {
	TYPE_NONE,
	I64,
	VEC_OBJECT,
	VEC_U8,
	STRING,
	BUFFER,
	UNDEFINED
};

/*   A port defines how the memory offset that is applied to it
 *   should be treated. */
struct port {
	enum memory_prot protection;
	enum memory_type type;
};

const char *port_type_string(enum memory_type type);
const char *port_prot_string(enum memory_prot prot);

struct port default_number_port(void);
struct port default_undefined_port(void);
struct port default_string_port(void);
struct port default_sequence_port(void);

/*   This rules tell how the return type of an 
 *   expression should be determined.
 *
 *       INHERIT: it is the same as the last parameter that was 
 *                executed.
 *          LINK: it is the same as the parameters if both are the same,
 *                or undefined.
 *          LEFT: it is the same as the left prameter 
 *         RIGHT: it is the same as the left prameter 
 */
enum expr_type_rule {
	RETURN_TYPE_RULE_INHERIT,
	RETURN_TYPE_RULE_LEFT,
	RETURN_TYPE_RULE_RIGHT,
};

/*   This rules tell how the return pointer of an 
 *   expression should be determined.
 *
 *        LEFT: it is the same as the left prameter 
 *       RIGHT: it is the same as the left prameter 
  *      YIELD: it is a new pointer
 */
enum expr_offset_rule {
	RETURN_OFFSET_RULE_LEFT,
	RETURN_OFFSET_RULE_RIGHT,
	RETURN_OFFSET_RULE_INHERIT,
	RETURN_OFFSET_RULE_YIELD
};

/*   The types of the parameters of an expression,
 *   as well as the type of its return value.
 */
struct expression_profile {
	struct port ret;
	struct {
		size_t	    count;
		struct port ports[8];
	} args;
};

#define __expr_prof_arg_count(__expr_prof) ((__expr_prof).args.count)
#define __expr_prof_arg_ports(__expr_prof) ((__expr_prof).args.ports)

struct expression_meta {
	enum expression_kind	  kind;
	struct expression_profile profile;
	struct {
		/* the rule for resolving the type */
		enum expr_type_rule type_rule;
		/* the rule for resolving the offset */
		enum expr_offset_rule offset_rule;
	};
};

extern struct expression_meta exp_meta_lookup[27];

/* returns a `struct expression_meta` given a 
 * `enum expression_kind`
 */
struct expression_meta *expr_profile_lookup(enum expression_kind kind
);

struct builtin {
	const char	      *iden;
	enum expr_type	       type;
	struct expression_meta meta;
};

extern const struct builtin builtin_lookup[2];

struct builtin *builtin_find(const char *lookup_name, size_t size);

#define __exp_profile_ret(__exp_profile)  ((__exp_profile)->ret)
#define __exp_profile_args(__exp_profile) ((__exp_profile)->args)

/*   This is the internal representation if a pointer.
 */
struct pointer {
	struct {
		bool	known_offset;
		ssize_t offset;
	};
	struct {
		bool	    known_port;
		struct port port;
	};
};

#define __pointer_known_port(__pointer)	  ((__pointer).known_port)
#define __pointer_known_offset(__pointer) ((__pointer).known_offset)
#define __pointer_offset(__pointer)	  ((__pointer).offset)
#define __pointer_port(__pointer)	  ((__pointer).port)

/*    This structure contains everything we need to know about an expression.
*/
struct attributes {
	enum expression_kind kind;
	struct {
		enum expr_type_rule   type_rule;
		enum expr_offset_rule offset_rule;
	};

	struct pointer pointer;

	struct {
		struct expression_profile actual; // expressed profile
		struct expression_profile expected; // expected profile
	} profiles; /* contains both the profile that would be expected
	from the expression and the profile that was expressed by the code.
	If both don't match, the expression is invalid. */
};

#define __exp_attr_kind(__exp_attr)   ((__exp_attr).kind)
#define __exp_attr_return(__exp_attr) ((__exp_attr).return_pointer)
#define __exp_attr_actual(__exp_attr) ((__exp_attr).profiles.actual)
#define __exp_attr_expected(__exp_attr) \
	((__exp_attr).profiles.expected)

enum frame_mask {
	CONSTEXPR, /*  The return value is known at compile time. */
	READONLY,  /*  Does not modify any variable. */
	CONST	   /*  Does not interact with any mutable variable. */
};

struct frame {
	struct vector  *locals;
	enum frame_mask mask;
};

struct expression {
	const struct token *origin;
	struct frame	    frame;
	struct attributes   attributes;
	struct builtin	   *builtin;

	union {
		struct vector	  *sequence; /* struct expression */
		struct pointer	   reference;
		struct expression *uniop;
		struct {
			struct expression *left;
			struct expression *right;
		} binop;
	};
};

#define __node_attributes(__expr_node_ptr) \
	((__expr_node_ptr)->attributes)
#define __node_attr_kind(__expr_node_ptr) \
	(__exp_attr_kind(__node_attributes(__expr_node_ptr)))

#define __node_token(__expr_node_ptr) ((__expr_node_ptr)->origin)

#define __node_token_kind(__expr_node_ptr) \
	(((__expr_node_ptr)->origin)->_kind)

#define __node_token_type(__expr_node_ptr) \
	(((__expr_node_ptr)->origin)->_type)

#define __node_locals(__expr_node_ptr) \
	((__expr_node_ptr)->frame.locals)

#define __node_frame_mask(__expr_node_ptr) \
	((__expr_node_ptr)->frame.mask)

#define __node_builtin(__expr_node_ptr) ((__expr_node_ptr)->builtin)

#define __node_as_sequence(__expr_node_ptr) \
	((__expr_node_ptr)->sequence)

#define __node_as_reference(__expr_node_ptr) \
	((__expr_node_ptr)->reference)

#define __node_as_uniop(__expr_node_ptr) ((__expr_node_ptr)->uniop)

#define __node_as_binop_l(__expr_node_ptr) \
	((__expr_node_ptr)->binop.left)

#define __node_as_binop_r(__expr_node_ptr) \
	((__expr_node_ptr)->binop.right)

#define __node_pointer(__expr_node_ptr) \
	((__expr_node_ptr)->attributes.pointer)

struct expression_meta *find_expression_meta(struct expression *node);

typedef struct {
	struct port port;
	const char *str;
	union {
		uint64_t		      any;
		int64_t			      number;
		dstr_t			     *string;
		struct vector /* uint8_t  */ *buffer;
		struct vector /* object_t */ *sequence;
		//xre_bytes_t  bytes;
	};

	void (*_drop)(void *);
	void (*_str)(void *);
	bool (*_test)(void *);
} object_t;

#define __object_port(__object)	       ((__object)->port)
#define __object_as_data(__object)     ((__object)->any)
#define __object_as_string(__object)   ((__object)->string)
#define __object_as_buffer(__object)   ((__object)->buffer)
#define __object_as_sequence(__object) ((__object)->sequence)
#define __object_as_number(__object)   ((__object)->number)

bool object_init(
	object_t **dest, struct port port, uint64_t default_value
);

struct runtime {
	struct expression *start;
	const char	  *name;
};

/*    Execute the ast
 */
bool runtime(struct ast *ast);

/* 
 *    Initialises the main data structure used at runtime.
 *    It contains the instructions as well as frame info such
 *    as local variables.
 */
bool ast_init(struct ast *ast, struct runtime *runtime);

void emit_ir(struct expression *node);

bool determine_variable_properties(struct expression *node);
bool resolve_return_locations(struct expression *node);
bool resolve_return_types(struct expression *node);

#endif
