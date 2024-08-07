#include "xre_memory.h"
#include "xre_assert.h"
#include "array.h"
#include <stdbool.h>

array_t *g_heap = NULL;
array_t *g_heap_stash = NULL;

size_t g_heap_allocd = 0;
size_t g_heap_in_use = 0;
size_t g_heap_freed = 0;

static inline bool heap_array_init(void)
{
	g_heap = array_create(sizeof(object_t), STACK_SIZE, object_drop);
	if (g_heap) {
		array_settle(g_heap);
		return (true);
	}
	return (false);
}

static inline void heap_array_deinit(void)
{
	array_kill(g_heap);
	g_heap = NULL;
}

static inline bool heap_stash_array_init(void)
{
	g_heap_stash = array_create(sizeof(size_t), 16, NULL);
	return (!!g_heap_stash);
}

static inline void heap_stash_array_deinit(void)
{
	array_kill(g_heap_stash);
	g_heap_stash = NULL;
}

// static inline size_t heap_get_free_slot(void)
// {
// 	static size_t slot;
// 	array_pop(g_heap_stash, &slot);
// 	return (slot);
// }

// static inline void heap_object_insert(object_t *object)
// {
// 	size_t slot = heap_get_free_slot();
// 	object->header.address = slot;
// 	array_tipex(g_heap, slot * sizeof(object_t), object, sizeof(object_t));
// }

// static inline bool heap_object_append(object_t *object)
// {
// 	object->header.address = array_size(g_heap);
// 	return (array_append(g_heap, object, 1));
// }

// static bool heap_allocate_object(object_t *object)
// {
// 	__return_val_if_fail__(g_heap, false);
// 	__return_val_if_fail__(object, false);

// 	// if (g_heap_allocd > g_heap_in_use) {
// 	// 	heap_object_insert(object);
// 	// 	return (true);
// 	// }

// 	// return (heap_object_append(object));
// }

bool heap_init(void)
{
	if (g_heap) {
		return (true);
	}

	if (!heap_array_init()) {
		return (false);
	}

	if (!heap_stash_array_init()) {
		heap_array_deinit();
		return (false);
	}

	return (true);
}

void heap_fini(void)
{
	heap_array_deinit();
	heap_stash_array_deinit();
}

// bool object_delete(object_t *object)
// {
// 	if (array_push(g_heap_stash, &__object_get_address(object))) {
// 		g_heap_in_use--;
// 		return (true);
// 	}

// 	return (false);
// }

// bool object_new(object_t *object)
// {
// 	if (!heap_allocate_object(object)) {
// 		return (false);
// 	}

// 	g_heap_allocd++;
// 	g_heap_in_use++;

// 	return (true);
// }

// object_t *object_get(size_t address)
// {
// 	return ((object_t *)array_at(g_heap, address));
// }
