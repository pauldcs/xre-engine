#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"

#define DEFAULT_NO_CHILD -1

xre_frame_t *frame_tree_g = NULL;

static bool alloc_frame_tree(size_t size)
{
	frame_tree_g = malloc(sizeof(xre_frame_t) * size);
	if (!frame_tree_g) {
		XRE_LOGGER(error, "Out of memory");
		return (false);
	}
	(void)memset(frame_tree_g, 0x00, sizeof(xre_frame_t) * size);
	return (true);
}

static void create_value_frame(xre_frame_t *frame, int64_t value)
{
	frame->state.value = value;
	frame->left_index = DEFAULT_NO_CHILD;
	frame->state.attrs = STATE_NUMBER;
	frame->right_index = DEFAULT_NO_CHILD;
}

static void create_string_frame(xre_frame_t *frame, const char *string)
{
	frame->state.string = (char *)string;
	frame->state.attrs = STATE_STRING;
	frame->state.attrs |= REFERENCE_FLAG;
	frame->left_index = DEFAULT_NO_CHILD;
	frame->right_index = DEFAULT_NO_CHILD;
}

static void create_identifier_frame(xre_frame_t *frame, const char *string)
{
	frame->identifier = (char *)string;
	frame->state.attrs = STATE_UNDEFINED;
	frame->state.attrs |= REFERENCE_FLAG;
	frame->left_index = DEFAULT_NO_CHILD;
	frame->right_index = DEFAULT_NO_CHILD;
}

static int frame_tree_create(xre_ast_t *ast)
{
	static int index = 0;
	xre_frame_t *frame = &frame_tree_g[index];
	int initial_index = index++;

	switch (ast->kind) {
	case __VAL__:
		create_value_frame(frame, ast->value);
		break;

	case __STRING_LITERAL__:
		create_string_frame(frame, ast->string);
		break;

	case __IDENTIFIER__:
		create_identifier_frame(frame, ast->string);
		break;

	case __NOT__:
	case __PRINT__:
		frame->state.attrs = STATE_UNDEFINED;
		frame->left_index = frame_tree_create(ast->uniop);
		frame->right_index = DEFAULT_NO_CHILD;
		break;

	default:
		frame->state.attrs = STATE_UNDEFINED;
		frame->left_index = frame_tree_create(ast->_binop.left);
		frame->right_index = frame_tree_create(ast->_binop.right);
		break;
	}

	frame->kind = ast->kind;
	frame->token = (xre_token_t *)&ast->token;

	return (initial_index);
}

static size_t get_ast_size(xre_ast_t *ast)
{
	size_t size = 1;

	switch (ast->kind) {
	case __VAL__:
	case __STRING_LITERAL__:
	case __IDENTIFIER__:
		return (size);

	case __NOT__:
	case __PRINT__:
		return (size + get_ast_size(ast->_binop.left));

	default:
		return (size + get_ast_size(ast->_binop.left) +
			get_ast_size(ast->_binop.right));
	}
}

int init_frame_tree(xre_ast_t *ast)
{
	__return_val_if_fail__(ast, -1);

	if (!alloc_frame_tree(get_ast_size(ast)))
		return (-1);

	return (frame_tree_create(ast));
}

void state_free(xre_state_t *state)
{
	if (IS_FLAG_SET(*state, STATE_ARRAY)) {
		array_kill(state->array);
	}
}

void deinit_frame_tree(xre_frame_t *frame)
{
	if (HAS_LEFT_CHILD(frame)) {
		deinit_frame_tree(LEFT_CHILD(frame));
	}

	if (HAS_RIGHT_CHILD(frame)) {
		deinit_frame_tree(RIGHT_CHILD(frame));
	}

	if (!IS_REF_STATE(frame->state)) {
		state_free(&frame->state);
	}
}
