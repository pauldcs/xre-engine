#include "xre_runtime.h"
#include "xre_stringf.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

static bool object_alloc_internal(object_t **object)
{
	*object = malloc(sizeof(object_t));
	if (!*object) {
		return (false);
	}
	(void)memset(*object, 0x00, sizeof(object_t));
	return (true);
}

// static void object_dealloc_internal(object_t **object)
// {
// 	(void)free((void *)*object);
// 	*object = NULL;
// }

static void object_setup_objattrs(object_t *object, int64_t objattrs)
{
	object->objattrs |= objattrs;
}

static void object_setup_methods(object_t *object)
{
	object->_drop = NULL;
	object->_str  = NULL;
	object->_test = NULL;
}

bool object_init(
	object_t **dest, int64_t objattrs, uint64_t default_value
)
{
	if (!object_alloc_internal(dest)) {
		return (false);
	}

	object_setup_objattrs(*dest, objattrs);
	object_setup_methods(*dest);

	(void)memcpy(&(*dest)->any, &default_value, sizeof(uint64_t));

	return (true);
}

#define BUFFER_SIZE 1024

const char *object_attr_to_str(int64_t attr)
{
	static char buffer[BUFFER_SIZE];

	size_t i = 0;
	(void)memset(&buffer, 0, BUFFER_SIZE);

	i +=
		cpyf(&buffer[i],
		     BUFFER_SIZE - i,
		     "%s",
		     attr & O_ATTR_READABLE ? "r" : "-");
	i +=
		cpyf(&buffer[i],
		     BUFFER_SIZE - i,
		     "%s",
		     attr & O_ATTR_MUTABLE ? "w" : "-");

	i += cpyf(&buffer[i], BUFFER_SIZE - i, " ");

	int64_t type = attr & O_TYPE_MASK;
	switch (type) {
	case O_TYPE_NUMBER:
		i += cpyf(&buffer[i], BUFFER_SIZE - i, "i64");
		break;

	case O_TYPE_SEQUENCE:
		i += cpyf(
			&buffer[i], BUFFER_SIZE - i, "vec(object)"
		);
		break;

	case O_TYPE_BUFFER:
		i += cpyf(&buffer[i], BUFFER_SIZE - i, "vec(u8)");
		break;

	case O_TYPE_STRING:
		i += cpyf(&buffer[i], BUFFER_SIZE - i, "dyn_string");
		break;

	case O_TYPE_UNDEFINED:
	default:
		i += cpyf(&buffer[i], BUFFER_SIZE - i, "???");
		break;
	}
	return (strdup(buffer));
}
