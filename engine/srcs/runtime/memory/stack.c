#include "xre_memory.h"
#include "xre_operations.h"
#include "xre_log.h"
#include "array.h"
#include "xre_xdp.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

array_t *g_stack = NULL;
size_t g_stack_ptr = 0;
size_t g_max_ptr = 0;

bool stack_init(void)
{
	g_stack_ptr = 0;
	g_stack = array_create(sizeof(object_t), STACK_SIZE, NULL);

	if (!g_stack) {
		return (false);
	}

	array_settle(g_stack);
	return (true);
}

void stack_fini(void)
{
	array_kill(g_stack);
	g_stack_ptr = 0;
	g_max_ptr = 0;
	g_stack = NULL;
}

bool stack_push(object_t *object)
{
	g_stack_ptr++;
	g_max_ptr++;
	return (array_push(g_stack, object));
}

const object_t *stack_top(void)
{
	return (const object_t *)array_tail(g_stack);
}

void stack_pop(object_t *ptr)
{
	g_stack_ptr--;
	array_pop(g_stack, ptr);
}

// void flags_to_string(int32_t flags) {
//   bool first = true;
//   if (flags & FLAG_REGISTER) {
//     (void)fprintf(stderr, "FLAG_REGISTER");
//     first = false;
//   }
//   if (flags & FLAG_ALLOC) {
//     if (first) {
//       first = false;
//     } else {
//       (void)fprintf(stderr, " | ");
//     }
//     (void)fprintf(stderr, "FLAG_ALLOC");
//   }
//   if (flags & FLAG_SEQUENCE) {
//     if (first) {
//       first = false;
//     } else {
//       (void)fprintf(stderr, " | ");
//     }
//     (void)fprintf(stderr, "FLAG_SEQUENCE");
//   }
//   if (flags & FLAG_REF) {
//     if (first) {
//       first = false;
//     } else {
//       (void)fprintf(stderr, " | ");
//     }
//     (void)fprintf(stderr, "FLAG_REF");
//   }
// }

void stack_debug(void)
{
	// (void)fprintf(stderr,
	//   "stack:\n"
	//   "  size: %d\n"
	//   "   ptr: %ld\n\n",
	//   STACK_SIZE,
	//   g_stack_ptr
	// );

	// size_t i = 0;
	// for (; i < g_max_ptr; i++) {
	//   const object_t *obj = (object_t *)array_at(g_stack, i);
	//   //(void)fprintf(stderr,
	//   //  " - in_use  %s\n"
	//   //  " - flags   ",
	//   //  i < g_stack_ptr ? "yes" : "no"
	//   //);
	//   //flags_to_string(obj->flags);
	//   (void)fprintf(stderr, "%ld", (int64_t)obj->data.ptr);
	//   // xre_xdp_dump(
	//   //   ((uint8_t *)g_stack->_ptr) + (i * sizeof(object_t)),
	//   //   sizeof(object_t),
	//   //   i * sizeof(object_t)
	//   // );
	//   (void)fprintf(stderr, "\n");
	// }
	// (void)fprintf(stderr, "\n");
}