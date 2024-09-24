#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_errors.h"
#include "xre_parse.h"
#include "dstr.h"
#include <stdbool.h>

struct frame {
	vec_t	 *variables /* object_t */;
	uintptr_t pointer;
};

struct meta {
	const char  *iden;
	xre_token_t *source;
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
		int64_t offset; // variable offset
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

#define OBJ_ATTR_MUTABLE   1 << 1
#define OBJ_ATTR_READABLE  1 << 2
#define OBJ_ATTR_REFERENCE 1 << 3
#define OBJ_ATTR_CONSTANT  1 << 4

#define OBJ_TYPE_NUMBER	   1 << 31
#define OBJ_TYPE_SEQUENCE  1 << 32
#define OBJ_TYPE_BUFFER	   1 << 33
#define OBJ_TYPE_STRING	   1 << 34
#define OBJ_TYPE_UNDEFINED 1 << 35

#define OBJ_ATTR_MASK 0x00000000ffffffff
#define OBJ_TYPE_MASK 0xffffffff00000000

typedef struct {
	int64_t objattrs;
	union {
		void		     *any;
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

struct runtime {
	struct statements *start;
	const char	  *name;
};

/*    Execute the ast
 */
bool runtime(xre_ast_t *ast);

/* 
 *    Initialises the main data structure used at runtime.
 *    It contains the instructions as well as frame info such
 *    as local variables.
 */
bool runtime_tree_init(xre_ast_t *ast, struct runtime *runtime);
void runtime_tree_put(struct statements *node);

enum builtin_type {
	BUILTIN_VALUE,
	BUILTIN_BINOP,
	BUILTIN_UNIOP,
};

struct builtin {
	const char	 *iden;
	enum builtin_type type;
	void		 *func;
};

extern struct builtin builtin_lookup[7];

bool		is_defined_builtin(const char *ptr, size_t size);
const char     *builtin_get_name(const char *ptr, size_t size);
xre_expr_type_t builtin_get_type(const char *ptr, size_t size);

#endif
