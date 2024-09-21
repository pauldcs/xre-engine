#include "vec.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
	int32_t id;
	int	offset;
} cache_entry_t;

vec_t *g_symtab	  = NULL;
vec_t *g_symcache = NULL;

int symtab_entry_create(const char *sym)
{
	size_t cache_size = vec_size(g_symcache);
	size_t i	  = 0;
	int    id	  = hash_string(sym);

	for (; i < cache_size; i++) {
		cache_entry_t *entry =
			(cache_entry_t *)vec_at(g_symcache, i);
		if (!entry) {
			return (-1);
		}

		if (id == (int)entry->id) {
			return entry->offset;
		}
	}

	cache_entry_t e = { .id = id, .offset = vec_size(g_symtab) };
	
	object_t obj = { 0 };
	object_undefined_init(&obj);

	if (!vec_push(g_symtab, &obj) ||
	    !vec_push(g_symcache, &e)) {
		return (-1);
	}

	return (e.offset);
}

bool symtab_init(void)
{
	if (g_symtab) {
		return (true);
	}

	g_symtab = vec_create(sizeof(object_t), STACK_SIZE, NULL);
	if (!g_symtab) {
		return (false);
	}

	g_symcache =
		vec_create(sizeof(cache_entry_t), STACK_SIZE, NULL);
	if (!g_symcache) {
		vec_kill(g_symtab);

		g_symtab   = NULL;
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

	vec_kill(g_symtab);
	vec_kill(g_symcache);

	g_symtab   = NULL;
	g_symcache = NULL;
}
