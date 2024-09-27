#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_errors.h"
#include "xre_nodes.h"
#include "dstr.h"
#include <stdbool.h>

struct frame {
	vec_t	 *variables /* object_t */;
	uintptr_t pointer;
};

struct meta {
	const char   *iden;
	struct token *source;
};

struct op_meta {
	enum expr_kind kind;
	int64_t	       _ret;
	int64_t	       _p1;
	int64_t	       _p2;
};

extern const struct op_meta op_meta_lookup[40];
struct op_meta		   *get_expr_types(enum expr_kind kind);

struct statements {
	bool (*_op)(struct statements *);

	struct meta meta;
	int64_t	    return_offset;
	int64_t	    return_type;
	int64_t	    ref_attrs;
	//struct frame frame;
	vec_t *frame;

	union {
		int64_t offset; // variable offset
		vec_t *children; // in the case of a sequence operation,
			// this i a verctor of children statements
		struct {
			struct statements *left;
			struct statements *right;
		};
	} self;
};

#define __statement_children(__statement) \
	((__statement)->self.children)

#define __statement_kind(__statement) \
	((__statement)->meta.source->_kind)

#define __statement_type(__statement) \
	((__statement)->meta.source->_type)

#define __statement_left(__statement) ((__statement)->self.left)

#define __statement_right(__statement) ((__statement)->self.right)

#define __statement_offset(__statement) ((__statement)->self.offset)

#define __statement_frame(__statement) ((__statement)->frame)


int64_t eval_return_offsets(struct statements *node);
int64_t eval_return_types(struct statements *node);
void eval_variable_flow(struct statements *node);
// struct xre_pp {

// }

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
		uint64_t	      any;
		dstr_t		     *string;
		vec_t /* uint8_t  */ *buffer;
		vec_t /* object_t */ *sequence;
		int64_t		      number;
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
	struct statements *start;
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
bool	runtime_tree_init(struct ast *ast, struct runtime *runtime);
int64_t runtime_borrow_check(struct statements *node);
void	runtime_tree_debug(struct statements *node);

#endif
