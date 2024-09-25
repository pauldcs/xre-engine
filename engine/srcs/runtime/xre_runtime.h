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

#define __binop_unfold_left(__statement_ptr) \
	((__statement_ptr)->self.left)
#define __binop_unfold_right(__statement_ptr) \
	((__statement_ptr)->self.right)

struct statements {
	bool (*_op)(struct statements *);

	struct meta meta;
	//struct frame frame;
	vec_t *frame;

	union {
		struct {
			int64_t offset; // variable offset
			int64_t attrs;
		};
		vec_t *children; // in the case of a sequence operation,
			// this i a verctor of children statements
		struct {
			struct statements *left;
			struct statements *right;
		};
	} self;
};

// struct xre_pp {

// }

#define OBJ_ATTR_MUTABLE   (1ULL << 1)
#define OBJ_ATTR_READABLE  (1ULL << 2)
#define OBJ_ATTR_REFERENCE (1ULL << 3)

// Type flags (upper 32 bits)
#define OBJ_TYPE_NUMBER    (1ULL << 33)
#define OBJ_TYPE_SEQUENCE  (1ULL << 34)
#define OBJ_TYPE_BUFFER    (1ULL << 35)
#define OBJ_TYPE_STRING    (1ULL << 36)
#define OBJ_TYPE_UNDEFINED (1ULL << 37)

// Masks
#define OBJ_ATTR_MASK 0x00000000FFFFFFFFULL
#define OBJ_TYPE_MASK 0xFFFFFFFF00000000ULL

typedef struct {
	int64_t objattrs;
	const char *str;
	union {
		uint64_t              any;
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

bool object_init(object_t **dest, int64_t objattrs, uint64_t default_value);
const char *object_to_str(object_t *object);
const char *object_attr_to_str(int64_t obj_attrs);
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
bool runtime_tree_init(struct ast *ast, struct runtime *runtime);
void runtime_tree_debug(struct statements *node);

#endif
