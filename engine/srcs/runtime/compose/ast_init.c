#include "xre_compiler.h"
#include "xre_nodes.h"
#include "defaults.h"
#include "vec.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <stdlib.h>
#include <stdbool.h>

/* DEFINE GLOBAL */
struct vector *global_scope_vector = NULL;

/* DEFINE GLOBAL */
size_t global_scope_vector_index = 0;

/* DEFINE GLOBAL */
static struct vector *global_local_cache = NULL;

/* DEFINE GLOBAL */
int64_t global_frame_index = -1;

struct local_cache_entry {
	const char *key;
	struct port port;
};

static bool ast_node_alloc(__ast_node **node)
{
	*node = malloc(sizeof(__ast_node));
	if (!*node) {
		return (false);
	}
	(void)memset(*node, 0x00, sizeof(__ast_node));
	return (true);
}

void ast_node_dealloc(__ast_node **node)
{
	(void)free((void *)*node);
	*node = NULL;
}

static bool local_cache_alloc(void)
{
	global_local_cache = vec_create(
		sizeof(struct local_cache_entry), 64, NULL
	);
	if (!global_local_cache) {
		return (false);
	}
	return (true);
}

static bool local_cache_push(const char *key, struct port port)
{
	struct local_cache_entry entry = { .key = key, .port = port };

	return (vec_push(global_local_cache, &entry));
}

static void local_cache_pop(void)
{
	vec_pop(global_local_cache, NULL);
}

static void local_cache_dealloc(void)
{
	(void)vec_kill(global_local_cache);
	global_local_cache = NULL;
}

static ssize_t is_cached(const char *key)
{
	size_t size = vec_size(global_local_cache);

	while (size--) {
		struct local_cache_entry *entry =
			(struct local_cache_entry *)
				vec_at(global_local_cache, size);

		if (strcmp(entry->key, key) == 0) {
			return (size);
		}
	}
	return (-1);
}

static void frame_push(const object_t *object)
{
	vec_push(global_scope_vector, object);
}

enum expression_kind expression_get_new_kind(enum expr_kind kind)
{
	switch (kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __VARIABLE__:
		return (REFERENCE);

	case __BUILTIN_CALL__:
		return (BUILTIN_CALL);

	case __SEQUENCE__:
	case __SEQUENCE_POINT__:
		return (SEQUENCE);

	case __LBRACK__:
	case __RBRACK__:
	case __LPAREN__:
	case __RPAREN__:
	case __ATTRIBUTE__:
	case __SCOPE_RESOLUTION__:
	case __START__:
	case __END__:
		return (IMPOSSIBLE);

	case __ADD__:
		return (ARITHMETHIC_ADD);

	case __SUB__:
		return (ARITHMETHIC_SUB);

	case __MUL__:
		return (ARITHMETHIC_MUL);

	case __DIV__:
		return (ARITHMETHIC_DIV);

	case __POW__:
		return (ARITHMETHIC_POW);

	case __MOD__:
		return (ARITHMETHIC_MOD);

	case __LT__:
		return (COMPARE_LT);

	case __GT__:
		return (COMPARE_GT);

	case __LE__:
		return (COMPARE_LE);

	case __GE__:
		return (COMPARE_GE);

	case __EQ__:
		return (COMPARE_EQ);

	case __NE__:
		return (COMPARE_NE);

	case __LSHIFT__:
		return (BIT_LSHIFT);

	case __RSHIFT__:
		return (BIT_RSHIFT);

	case __AND__:
		return (LOGIC_AND);

	case __OR__:
		return (LOGIC_OR);

	case __DO__:
		return (LOGIC_DO);

	case __ELSE__:
		return (LOGIC_ELSE);

	case __NOT__:
		return (LOGIC_NOT);

	case __LOOP__:
		return (LOGIC_LOOP);

	case __CLOSURE__:
		return (CLOSURE);

	case __BAND__:
		return (BIT_AND);

	case __BOR__:
		return (BIT_OR);

	case __BXOR__:
		return (BIT_XOR);

	case __ASSIGN__:
		return (ASSIGN);

	case __SEPARATOR__:
		return (SEPARATOR);
	}
}

static bool expression_is_scope_modifier(const __ast_node *node)
{
	switch (__node_token_kind(node)) {
	case __DO__:
	case __ELSE__:
	case __AND__:
	case __OR__:
	case __LOOP__:
	case __CLOSURE__:
		return (true);

	default:
		return (false);
	}
}

static bool _is_scope_change(const __ast_node *parent)
{
	if (!parent) {
		return (true);
	}

	return (expression_is_scope_modifier(parent));
}

static const char *format_ast_value(const struct ast *ast)
{
	switch (ast->kind) {
	case __VARIABLE__:
		return (format_string("%s", ast->string));

	case __VAL__:
		return (format_string("%d", ast->value));

	case __STRING_LITERAL__:
		return (format_string("'%s'", ast->string));

	default:
		return (format_string("_"));
	}
}

struct expression_meta *find_expression_meta(__ast_node *node)
{
	struct expression_meta *meta = NULL;
	if (__node_attr_kind(node) == BUILTIN_CALL) {
		const char *builtin_name = __node_builtin(node)->iden;
		struct builtin *builtin	 = builtin_find(
			 builtin_name, strlen(builtin_name)
		 );
		meta = &builtin->meta;
	} else {
		meta = expr_profile_lookup(__node_attr_kind(node));
	}
	return (meta);
}

static void expression_init(__ast_node *node, struct ast *ast)
{
	enum expression_kind kind =
		expression_get_new_kind(ast->kind);

	struct expression_meta *meta = find_expression_meta(node);

	if (kind == REFERENCE) {
		goto end;
	}

	struct attributes attr = {
		.kind = kind,
			.o_rule = meta->o_rule,
			.t_rule = meta->t_rule,
			.pointer = {
				.known_offset = false,
				.known_port = false,
			},
		.profiles = {
			.actual = {
				.args.count = 0,
				.args.ports = { __undefined_port }
			}
		}
	};

	size_t argc = __expr_prof_arg_count(meta->profile);
	__expr_prof_arg_count(__exp_attr_expected(attr)) = argc;

	while (argc--) {
		__expr_prof_arg_ports(__exp_attr_expected(attr)
		)[argc] = __expr_prof_arg_ports(meta->profile)[argc];
	}
	__node_attributes(node) = attr;

end:
	__node_locals(node) = vec_create(sizeof(object_t), 64, NULL);
	__node_token(node)  = &ast->token;
}

void frame_propagate_diff(
	__ast_node *node, int64_t frame_index_guard, int diff
)
{
	if (global_frame_index == -1) {
		global_frame_index = 0;
	}

	if (vec_size(__node_locals(node))) {
		global_frame_index++;
	}

	if (__node_token_kind(node) == __SEQUENCE_POINT__) {
		size_t sequence_size =
			vec_size(__node_as_sequence(node));

		for (size_t i = 0; i < sequence_size; i++) {
			frame_propagate_diff(
				vec_access(
					__node_as_sequence(node), i
				),
				frame_index_guard,
				diff
			);
		}
		return;
	}

	ssize_t *offset;

	switch (__node_token_type(node)) {
	case EXPR_TYPE_VALUE:
		offset = &__node_pointer(node).offset;
		if (global_frame_index &&
		    *offset >= frame_index_guard) {
			*offset += diff;
		}
		break;

	case EXPR_OP_TYPE_UNIOP:
		frame_propagate_diff(
			__node_as_binop_l(node),
			frame_index_guard,
			diff
		);
		break;

	case EXPR_OP_TYPE_BINOP:
		frame_propagate_diff(
			__node_as_binop_l(node),
			frame_index_guard,
			diff
		);
		frame_propagate_diff(
			__node_as_binop_r(node),
			frame_index_guard,
			diff
		);
		break;

	default:
		break;
	}
}

static void create_reference_pointer(
	__ast_node *node, struct port *src_port, size_t offset
)
{
	__node_attr_kind(node)				  = REFERENCE;
	__pointer_known_port(__node_as_reference(node))	  = true;
	__pointer_known_offset(__node_as_reference(node)) = true;
	__pointer_port(__node_as_reference(node)).prot =
		src_port->prot;
	__pointer_port(__node_as_reference(node)).type =
		src_port->type;
	__pointer_offset(__node_as_reference(node)) = offset;
}

static bool ast_create(
	struct ast *ast, __ast_node **statements, bool is_scope_change
)
{
	const char *format	       = NULL;
	ssize_t	    self_cached_offset = -1;
	size_t	    self_offset	       = 0;
	size_t	    n;

	if (!ast_node_alloc(statements)) {
		return (false);
	}

	__ast_node *node = *statements;

	(void)expression_init(node, ast);

	if (global_scope_vector == NULL) {
		global_scope_vector = __node_locals(node);
	}

	struct vector *scope_vector_save = global_scope_vector;
	size_t scope_vector_index_save	 = global_scope_vector_index;

	if (is_scope_change) {
		global_scope_vector_index +=
			vec_size(global_scope_vector);
		global_scope_vector = __node_locals(node);
	}

	if (__node_token_kind(node) == __BUILTIN_CALL__) {
		__node_builtin(node) = builtin_find(
			__node_token(node)->_ptr,
			__node_token(node)->_len
		);

	} else if (__node_token_kind(node) == __SEQUENCE_POINT__) {
		n = vec_size(ast->seq);

		__node_as_sequence(node) =
			vec_create(sizeof(__ast_node), n, NULL);

		for (size_t i = 0; i < n; i++) {
			__ast_node *exp = NULL;

			if (!ast_create(
				    vec_access(ast->seq, i),
				    &exp,
				    is_scope_change
			    )) {
				return (false);
			}

			vec_push(__node_as_sequence(node), exp);
		}
		goto end;
	}

	size_t frame_size_guard = 0;
	switch (ast->type) {
	case EXPR_TYPE_VALUE:
		format		   = format_ast_value(ast);
		self_cached_offset = is_cached(format);
		self_offset	   = -1;

		if (self_cached_offset == -1) {
			self_offset = global_scope_vector_index +
				      vec_size(scope_vector_save);

		} else {
			self_offset = self_cached_offset;

			struct local_cache_entry *entry =
				(struct local_cache_entry *)
					vec_at(global_local_cache,
					       self_offset);

			create_reference_pointer(
				node, &entry->port, self_offset
			);
		}

		if (self_cached_offset == -1) {
			object_t *object = NULL;

			switch (ast->kind) {
			case __VAL__:
				(void)object_init(
					&object,
					default_number_port(),
					(uint64_t)ast->value
				);
				break;

			case __STRING_LITERAL__:
				(void)object_init(
					&object,
					default_string_port(),
					(uint64_t)ast->string
				);
				break;

			case __SEQUENCE__:
				(void)object_init(
					&object,
					default_sequence_port(),
					(uint64_t)ast->seq
				);

			case __VARIABLE__:
			default:
				(void)object_init(
					&object,
					default_undefined_port(),
					(uint64_t)NULL
				);
				break;
			}

			create_reference_pointer(
				node, &object->port, self_offset
			);

			local_cache_push(format, object->port);
			object->str = format;
			frame_push(object);
		} else {
			/* nothing */
		}

		break;

	case EXPR_OP_TYPE_UNIOP:
		if (!ast_create(
			    ast->_binop.left,
			    &__node_as_binop_l(node),
			    false
		    )) {
			return (false);
		}

		break;

	case EXPR_OP_TYPE_BINOP:

		if (!ast_create(
			    ast->_binop.left,
			    &__node_as_binop_l(node),
			    false
		    )) {
			return (false);
		}

		frame_size_guard = vec_size(global_scope_vector);

		if (!ast_create(
			    ast->_binop.right,
			    &__node_as_binop_r(node),
			    _is_scope_change(node)
		    )) {
			return (false);
		}

		int diff = vec_size(global_scope_vector) -
			   frame_size_guard;
		if (diff) {
			frame_propagate_diff(
				__node_as_binop_l(node),
				frame_size_guard,
				diff
			);
			global_frame_index = -1;
		}

		break;

	default:
		break;
	}

end:
	if (is_scope_change) {
		global_scope_vector	  = scope_vector_save;
		global_scope_vector_index = scope_vector_index_save;
		size_t n = vec_size(__node_locals(node));
		while (n--) {
			local_cache_pop();
		}
	}

	*statements = node;
	return (true);
}

bool ast_init(struct ast *ast, struct runtime *runtime)
{
	__trigger_bug_if(ast == NULL);
	__trigger_bug_if(runtime == NULL);

	bool ret = false;

	global_scope_vector_index = 0;
	global_frame_index	  = -1;
	global_scope_vector	  = NULL;
	global_local_cache	  = NULL;

	if (local_cache_alloc()) {
		ret = ast_create(ast, &runtime->start, true);
		local_cache_dealloc();
	}
	return (ret);
}
