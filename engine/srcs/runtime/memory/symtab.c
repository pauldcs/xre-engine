#include "vec.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

vec_t *g_symbols = NULL;
struct symbol {
	const char *name;
	object_t    object;
};

static inline bool symbol_offset_get(const char *name, size_t *offset)
{
	size_t	       size = vec_size(g_symbols);
	struct symbol *s;
	while (size--) {
		s = (struct symbol *)vec_at(g_symbols, size);
		if (strcmp(s->name, name) == 0) {
			*offset = size;
			return (true);
		}
	}
	return (false);
}

bool symbol_access(const char *name, struct symbol **symbol)
{
	size_t offset = 0;
	if (!symbol_offset_get(name, &offset)) {
		return (false);
	}

	*symbol = vec_access(g_symbols, offset);
	return (true);
}

void symbol_get_unsafe(size_t offset, struct symbol **symbol)
{
	*symbol = vec_access(g_symbols, offset);
}

bool symbol_register(const char *name, size_t *position)
{
	struct symbol sym;

	if (!symbol_offset_get(name, position)) {
		return (false);
	}

	sym.name = name;
	object_undefined_init(&sym.object);

	*position = vec_size(g_symbols);

	return (vec_push(g_symbols, &sym));
}

// bool symbols_allocate(size_t count)
// {
// 	size_t size = count;
// 	object_t object = { 0 };

// 	if (vec_uninitialized_size(g_symbols) >= count) {
// 		goto copy;
// 	} else {
// 		if (unlikely(!vec_adjust(g_symbols, count))) {
// 			return (false);
// 		}
// 	}

// copy:
// 	while (size--) {
// 		object_undefined_init(&object);
// 		(void)vec_push(g_symbols, &object);
// 	}

// 	return (unlikely(!vec_append_from_capacity(g_symbols, count)));
// }

void symbols_deallocate(size_t count)
{
	while (count--) {
		(void)vec_pop(g_symbols, NULL);
	}
}

bool symtab_init(void)
{
	if (g_symbols) {
		return (true);
	}

	g_symbols =
		vec_create(sizeof(struct symbol), STACK_SIZE, NULL);
	if (!g_symbols) {
		return (false);
	}

	return (true);
}

void symtab_fini(void)
{
	if (!g_symbols) {
		return;
	}

	vec_kill(g_symbols);
	g_symbols = NULL;
}
