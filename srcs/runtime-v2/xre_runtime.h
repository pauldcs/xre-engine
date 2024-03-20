#ifndef __XRE_RUNTIME_H__
# define __XRE_RUNTIME_H__

# include "xre_errors.h"
# include "xre_parse.h"

/*    All the different types of possible statements.
 */
#define FLAG(name, shift) (1LL << (shift))

#define STMT_IDENTIFIER        FLAG(IDENTIFIER, 1)
#define STMT_VAL               FLAG(VAL, 2)
#define STMT_STRING_LITERAL    FLAG(STRING_LITERAL, 3)

#define STMT_ADD               FLAG(ADD, 4)
#define STMT_SUB               FLAG(SUB, 5)
#define STMT_MUL               FLAG(MUL, 6)
#define STMT_DIV               FLAG(DIV, 7)
#define STMT_POW               FLAG(POW, 8)
#define STMT_MOD               FLAG(MOD, 9)
#define STMT_LSHIFT            FLAG(LSHIFT, 10)
#define STMT_RSHIFT            FLAG(RSHIFT, 11)
#define STMT_BAND              FLAG(BAND, 12)
#define STMT_BOR               FLAG(BOR, 13)
#define STMT_BXOR              FLAG(BXOR, 14)

#define STMT_LT                FLAG(LT, 15)
#define STMT_GT                FLAG(GT, 16)
#define STMT_LE                FLAG(LE, 17)
#define STMT_GE                FLAG(GE, 18)
#define STMT_EQ                FLAG(EQ, 19)
#define STMT_NE                FLAG(NE, 20)
#define STMT_NOT               FLAG(NOT, 21)

#define STMT_AND               FLAG(AND, 22)
#define STMT_OR                FLAG(OR, 23)
#define STMT_LOOP              FLAG(LOOP, 24)
#define STMT_DO                FLAG(DO, 25)
#define STMT_ELSE              FLAG(ELSE, 26)

#define STMT_ASSIGN            FLAG(ASSIGN, 27)
#define STMT_ADD_ASSIGN        FLAG(ADD_ASSIGN, 28)
#define STMT_SUB_ASSIGN        FLAG(SUB_ASSIGN, 29)
#define STMT_MUL_ASSIGN        FLAG(MUL_ASSIGN, 30)
#define STMT_DIV_ASSIGN        FLAG(DIV_ASSIGN, 31)
#define STMT_MOD_ASSIGN        FLAG(MOD_ASSIGN, 32)
#define STMT_POW_ASSIGN        FLAG(POW_ASSIGN, 33)

#define STMT_SEQUENCE_POINT    FLAG(SEQUENCE_POINT, 34)
#define STMT_SEPARATOR         FLAG(SEPARATOR, 35)
#define STMT_MEMBER            FLAG(MEMBER, 36)
#define STMT_INJECT            FLAG(INJECT, 37)
#define STMT_ANNOTATE          FLAG(ANNOTATE, 38)
#define STMT_SCOPE_RESOLUTION  FLAG(SCOPE_RESOLUTION, 39)

#define GROUP_MASK(group) ( \
    ((1LL << (group##_END - group##_START + 1)) - 1) << group##_START \
)

#define VALUE_START 1
#define VALUE_END   3
#define VALUE_GROUP_MASK GROUP_MASK(VALUE)

#define ARITHMETIC_START 4
#define ARITHMETIC_END   14
#define ARITHMETIC_GROUP_MASK GROUP_MASK(ARITHMETIC)

#define RELATIONAL_START 15
#define RELATIONAL_END   21
#define RELATIONAL_GROUP_MASK GROUP_MASK(RELATIONAL)

#define LOGICAL_START 22
#define LOGICAL_END   26
#define LOGICAL_GROUP_MASK GROUP_MASK(LOGICAL)

#define ASSIGNMENT_START 27
#define ASSIGNMENT_END   33
#define ASSIGNMENT_GROUP_MASK GROUP_MASK(ASSIGNMENT)

#define SPECIALS_START 34
#define SPECIALS_END   39
#define SPECIAL_GROUP_MASK GROUP_MASK(SPECIAL)


/*    Member functions for "object_t"
 */
typedef struct object_s object_t;

typedef void (*destructor)(object_t *);
typedef void (*reprfunc)(object_t *);
typedef bool (*testfunc)(object_t *);

/*    A representation of a value during runtime.
 */
struct object_s {
  destructor dealloc; // frees the object
  reprfunc repr; // used by the print function to display the object
  testfunc test; // returns true/false based on the truthiness of the value
  struct {
    size_t size; // the size of the buffer
    void *ptr; // a pointer to the buffer
  } data;
};

typedef struct stmt_tree_s stmt_tree_t;
typedef int stmt_t;

/*    The parsed AST is tranformed into this 'stmt_tree_t'
 *    which is essencially the same except in an 
 *    array as it gets traversed a lot during runtime.
 */
typedef void* oper_fptr_t;
typedef bool (*prologue_t)(stmt_tree_t *);

typedef struct op_info_s {
  //int64_t type; // the type of the statement (1 or many STMT_*)
  prologue_t init; // initializes the stack to run the 'fptr' function
  oper_fptr_t fptr; // a pointer to a function that can run the operation
} op_info_t;

struct stmt_tree_s {
  op_info_t op;
  xre_token_t *orig; // the code fragment from which this statement is parsed from
  union {
    int64_t value; // the value that the token represents
    char *string; // the string that the token represents
    struct {
      stmt_t left; // index of the left child branch
      stmt_t right; // index of the right child branch
    } br;
  };
};

/*    Execute the ast
 */
bool xre_runtime(xre_ast_t *ast);

/*    Allocated a new stmt_tree_t from a xre_ast_t
 */
stmt_tree_t *stmt_tree_init(xre_ast_t *ast);

/*    Deallocate the whole stmt_tree_t
 */
void stmt_tree_fini(stmt_tree_t *stmts);


/*    Global variable that holds the last set error.
 */
extern err_notif_t _error;

/*    sets the '_error' global struct with the appropriate info
 *    given a 'error_type_e'. 
 */
void set_error(stmt_tree_t *origin, error_type_e type);

#endif
