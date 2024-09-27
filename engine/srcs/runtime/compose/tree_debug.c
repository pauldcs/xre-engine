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

static const char *format_token(struct token *token)
{
	return (format_string(
		"$%s \"%s\" `%02d:%02d:%02d:%02d`",
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

void runtime_tree_debug(struct statements *node)
{
	if (__statement_type(node) != EXPR_TYPE_VALUE) {
		if (node->return_offset != -1) {
			print_info(format_string(
				"%s +%04d # %s", format_token(node->meta.source), node->return_offset, object_attr_to_str(node->return_type)
			));
		} else {
			print_info(format_string(
				"%s # %s", format_token(node->meta.source), object_attr_to_str(node->return_type)
			));
		}
	}

	if (vec_size(__statement_frame(node))) {
		sp += vec_size(__statement_frame(node));
	}

	size_t i = 0;
	while (i < vec_size(__statement_frame(node))) {
		object_t *o = (object_t *)vec_at(
			__statement_frame(node), i++
		);
		const char *str = object_attr_to_str(o->objattrs);
		print_info(format_string(
			" (+) alloc +%04zu # @var_%s (0x%llx) %s",
			++counter - 1,
			o->str,
			o->any,
			str
		));
	}

	i = 0;
	if (__statement_kind(node) == __SEQUENCE_POINT__) {
		d++;
		while (i < vec_size(__statement_children(node))) {
			runtime_tree_debug((struct statements *)vec_at(
				__statement_children(node), i++
			));
		}
		d--;
		goto end;
	}

	switch (node->meta.source->_type) {
	case EXPR_TYPE_VALUE:
		print_info(format_string(
			"ref &%04d # %s",
			__statement_offset(node),
			object_attr_to_str(node->ref_attrs)
		));
		break;

	case EXPR_OP_TYPE_UNIOP:
		d++;
		(void)runtime_tree_debug(__statement_left(node));
		d--;

		break;

	case EXPR_OP_TYPE_BINOP:
		d++;
		(void)runtime_tree_debug(__statement_left(node));
		(void)runtime_tree_debug(__statement_right(node));
		d--;

		break;

	default:

		break;
	}

end:
	i = 0;
	while (i++ < vec_size(__statement_frame(node))) {
		print_info(format_string(
			" (-) drop +%04zu", counter-- - 1
		));
	}

	if (vec_size(__statement_frame(node))) {
		sp -= vec_size(__statement_frame(node));
	}
}
