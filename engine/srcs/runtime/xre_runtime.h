#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_errors.h"
#include "xre_builtin.h"
#include "xre_args.h"
#include "xre_nodes.h"
#include "dstr.h"
#include <stdbool.h>

// struct frame {
// 	struct vector *variables /* object_t */;
// 	uintptr_t      pointer;
// };

struct meta {
	const char   *iden;
	struct token *source;
};

struct operation_info {
	enum expr_kind kind;
	int64_t	       _ret;
	int64_t	       _p1;
	int64_t	       _p2;
};

extern const struct operation_info op_info_lookup[40];

struct operation_info *operation_info_lookup_kind(enum expr_kind kind
);

// enum operand {
// 	P_REFERENCE,
// 	P_SEQUENCE,
// 	P_WEAK_SEQUENCE,
// 	P_CLOSURE,
// 	P_USER_UNIOP,
// 	P_USER_BINOP,
// 	P_ARITHMETHIC_ADD,
// 	P_ARITHMETHIC_SUB,
// 	P_ARITHMETHIC_MUL,
// 	P_ARITHMETHIC_MOD,
// 	P_ARITHMETHIC_DIV,
// 	P_ARITHMETHIC_POW,
// 	P_LOGIC_AND,
// 	P_LOGIC_OR,
// 	P_LOGIC_DO,
// 	P_LOGIC_ELSE,
// 	P_LOGIC_LOOP,
// 	P_LOGIC_NOT,
// 	P_BIT_AND,
// 	P_BIT_OR,
// 	P_BIT_XOR,
// 	P_LSHIFT,
// 	P_RSHIFT,
// 	P_COMPARE_LT,
// 	P_COMPARE_GT,
// 	P_COMPARE_LE,
// 	P_COMPARE_GE,
// 	P_COMPARE_EQ,
// 	P_COMPARE_NE,
// };

/*
 *    Used to access memory at runtime.
*/
struct port {
	int64_t offset;
	int64_t pmask;
};

/*  If the unknown return value was known at compile time,
 *  it could be replaced by it.
 */
#define F_ATTR_PURE  (1ULL << 1)

/*  The return value is known at compile time.
 */
#define F_ATTR_CONSTEXPR  (1ULL << 2)

/*  Does not modify any variable.
 */
#define F_ATTR_READONLY (1ULL << 3)

/*  Does not interact with any mutable variable.
 */
#define F_ATTR_CONST (1ULL << 4)

/*  Its return type is the same as its input
 *  parameter(s).
 */
#define F_ATTR_LINK (1ULL << 5)


struct frame {
	struct vector *locals;
	int64_t        fmask;
	// size_t	       alive_size;
	// size_t	       hidden_size;
};

#define __ast_node struct expression

struct expression {
	//enum operand    iden;
	//enum operand    operand;
	struct token   *origin;
	struct builtin *builtin;
	struct frame	frame;
	struct port	dest;

	union {
		struct vector *sequence;
		__ast_node    *uniop;
		struct port    reference;

		struct {
			__ast_node *left;
			__ast_node *right;
		} binop;
	};
};

#define __expression_sequence(__expression) ((__expression)->sequence)

#define __expression_kind(__expression) \
	((__expression)->origin->_kind)

#define __expression_type(__expression) \
	((__expression)->origin->_type)
#define __expression_operand(__expression) ((__expression)->operand)

#define __expression_builtin(__expression) ((__expression)->builtin)
#define __expression_origin(__expression)  ((__expression)->origin)
#define __expression_dest(__expression)	   ((__expression)->dest)
#define __expression_binop_left(__expression) \
	((__expression)->binop.left)
#define __expression_binop_right(__expression) \
	((__expression)->binop.right)

#define __expression_frame_locals(__expression) \
	((__expression)->frame.locals)
#define __expression_frame_user(__expression) \
	((__expression)->frame.total_user)
#define __expression_frame_meta(__expression) \
	((__expression)->frame.total_meta)

#define __expression_dest_offset(__expression) \
	((__expression)->dest.offset)
#define __expression_dest_pmask(__expression) \
	((__expression)->dest.pmask)
#define __expression_ref_offset(__expression) \
	((__expression)->reference.offset)
#define __expression_ref_pmask(__expression) \
	((__expression)->reference.pmask)

// Attribute flags (bits 0-15)
#define O_ATTR_MUTABLE	(1ULL << 1)
#define O_ATTR_READABLE (1ULL << 12)

// Branch result flags (bits 16-31)
#define O_BR_RESULT_LEFT  (1ULL << 16)
#define O_BR_RESULT_RIGHT (1ULL << 17)
#define O_BR_RESULT_ANY	  (1ULL << 18)

// Type flags (bits 32-63)
#define O_TYPE_NUMBER	 (1ULL << 32)
#define O_TYPE_SEQUENCE	 (1ULL << 33)
#define O_TYPE_BUFFER	 (1ULL << 34)
#define O_TYPE_STRING	 (1ULL << 35)
#define O_TYPE_UNDEFINED (1ULL << 36)
#define O_TYPE_NONE	 (1ULL << 37)

// Masks
#define O_ATTR_MASK 0x000000000000FFFFULL
#define O_BR_MASK   0x00000000FFFF0000ULL
#define O_TYPE_MASK 0xFFFFFFFF00000000ULL
#define O_META_

typedef struct {
	int64_t	    objattrs;
	const char *str;
	union {
		uint64_t		      any;
		dstr_t			     *string;
		struct vector /* uint8_t  */ *buffer;
		struct vector /* object_t */ *sequence;
		int64_t			      number;
		//xre_bytes_t  bytes;
	};

	void (*_drop)(void *);
	void (*_str)(void *);
	bool (*_test)(void *);
} object_t;

#define __object_objattrs(__object) ((__object)->objattrs)

#define __object_str(__object) ((__object)->str)

#define __object_as_data(__object) ((__object)->any)

#define __object_as_string(__object) ((__object)->string)

#define __object_as_buffer(__object) ((__object)->buffer)

#define __object_as_sequence(__object) ((__object)->sequence)

#define __object_as_number(__object) ((__object)->number)

bool object_init(
	object_t **dest, int64_t objattrs, uint64_t default_value
);
const char *object_attr_to_str(int64_t attr);

struct runtime {
	__ast_node *start;
	const char *name;
};

/*    Execute the ast
 */
bool runtime(struct ast *ast);

/* 
 *    Initialises the main data structure used at runtime.
 *    It contains the instructions as well as frame info such
 *    as local variables.
 */
bool runtime_tree_init(struct ast *ast, struct runtime *runtime);

void emit_ir(__ast_node *node, bool is_left);

bool determine_variable_properties(__ast_node *node);
bool resolve_return_locations(__ast_node *node);
bool resolve_return_types(__ast_node *node);

#endif
