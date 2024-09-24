#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

int	    d = 0;
static void print_info(const char *info)
{
	size_t i = d;
	while (i--) {
		(void)write(STDERR_FILENO, "    ", 4);
	}

	(void)write(STDERR_FILENO, info, strlen(info));
	(void)write(STDERR_FILENO, "\n", 1);
}

static const char *format_token(xre_token_t *token)
{
	return (format_string(
		"$%s \"%s\" \t# %02d:%02d:%02d:%02d",
		expr_type_to_string(token->_type),
		expr_kind_to_string(token->_kind),
		token->_line,
		token->_cols,
		token->_len,
		token->_depth
	));
}

static int counter = 0;
static int sp	   = 0;

void runtime_tree_put(struct statements *node)
{
	if (node->meta.source->_type != EXPR_TYPE_VALUE) {
		print_info(format_string(
			"%s", format_token(node->meta.source)
		));
	}

	if (vec_size(node->frame)) {
		sp += vec_size(node->frame);
	}
	size_t i = 0;
	while (i < vec_size(node->frame)) {
		print_info(format_string(
			" reserve +%04zu \t# __%s",
			++counter - 1,
			*(char **)vec_at(node->frame, i++)
		));
	}

	i = 0;
	if (node->meta.source->_kind == __SEQUENCE_POINT__) {
		d++;
		while (i < vec_size(node->self.children)) {
			runtime_tree_put((struct statements *)vec_at(
				node->self.children, i++
			));
		}
		d--;
		goto end;
	}

	switch (node->meta.source->_type) {
	case EXPR_TYPE_VALUE:
		print_info(
			format_string("ref &%04zu", node->self.offset)
		);
		break;

	case EXPR_OP_TYPE_UNIOP:
		d++;
		(void)runtime_tree_put(__binop_unfold_left(node));
		d--;

		break;

	case EXPR_OP_TYPE_BINOP:
		d++;
		(void)runtime_tree_put(__binop_unfold_left(node));
		(void)runtime_tree_put(__binop_unfold_right(node));
		d--;

		break;

	default:

		break;
	}

end:
	i = 0;
	while (i++ < vec_size(node->frame)) {
		print_info(
			format_string(" drop +%04zu", counter-- - 1)
		);
	}

	if (vec_size(node->frame)) {
		sp -= vec_size(node->frame);
	}
}
