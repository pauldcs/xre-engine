#include "array.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_utils.h"
#include "xre_xdp.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
	int32_t id;
	int offset;
} cache_entry_t;

array_t *g_symtab = NULL;
array_t *g_symcache = NULL;

int symtab_create_entry(const char *sym)
{
	size_t cache_size = array_size(g_symcache);
	size_t i = 0;
	int id = hash_string(sym);

	for (; i < cache_size; i++) {
		cache_entry_t *entry = (cache_entry_t *)array_at(g_symcache, i);
		if (!entry) {
			return (-1);
		}

		if (id == (int)entry->id) {
			return entry->offset;
		}
	}

	cache_entry_t e = { .id = id, .offset = array_size(g_symtab) };

	if (!array_push(g_symtab, object_create_undefined()) ||
	    !array_push(g_symcache, &e)) {
		return (-1);
	}

	return (e.offset);
}

object_t *symtab_get_entry(int index)
{
	return array_unsafe_access(g_symtab, index);
}

bool symtab_init(void)
{
	if (g_symtab) {
		return (true);
	}

	g_symtab = array_create(sizeof(object_t), STACK_SIZE, NULL);
	if (!g_symtab) {
		return (false);
	}

	g_symcache = array_create(sizeof(cache_entry_t), STACK_SIZE, NULL);
	if (!g_symcache) {
		array_kill(g_symtab);
		g_symtab = NULL;
		g_symcache = NULL;
		return (false);
	}

	return (true);
}

void symtab_fini(void)
{
	if (!g_symtab || !g_symcache) {
		return;
	}
	array_kill(g_symtab);
	array_kill(g_symcache);

	g_symtab = NULL;
	g_symcache = NULL;
}
