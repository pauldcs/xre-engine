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
	if (objattrs == -1) {
		object->objattrs |=
			(OBJ_ATTR_READABLE |
			 OBJ_TYPE_NUMBER);
	} else {
		object->objattrs |= objattrs;
	}
}

static void object_setup_methods(object_t *object)
{
	object->_drop = NULL;
	object->_str  = NULL;
	object->_test = NULL;
}

bool object_init(object_t **dest, int64_t objattrs, uint64_t default_value)
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
const char *object_attr_to_str(int64_t obj_attrs) {
        static char buffer[BUFFER_SIZE];

        size_t i = 0;
        (void)memset(&buffer, 0, BUFFER_SIZE);

        int64_t attrs = obj_attrs & OBJ_ATTR_MASK;
        i += cpyf(&buffer[i], BUFFER_SIZE - i, "attrs: %s", attrs & OBJ_ATTR_READABLE ? "R" : "-");
        i += cpyf(&buffer[i], BUFFER_SIZE - i, "%s", attrs & OBJ_ATTR_MUTABLE ? "W" : "-");
        i += cpyf(&buffer[i], BUFFER_SIZE - i, "%s", attrs & OBJ_ATTR_REFERENCE ? "*" : "-");
        return (strdup(buffer));
}

const char *object_to_str(object_t *object) {
        static char buffer[BUFFER_SIZE];

        size_t i = 0;
        (void)memset(&buffer, 0, BUFFER_SIZE);

        int64_t attrs = object->objattrs & OBJ_ATTR_MASK;
        i += cpyf(&buffer[i], BUFFER_SIZE - i, "attrs: %s", attrs & OBJ_ATTR_READABLE ? "R" : "-");
        i += cpyf(&buffer[i], BUFFER_SIZE - i, "%s", attrs & OBJ_ATTR_MUTABLE ? "W" : "-");
        i += cpyf(&buffer[i], BUFFER_SIZE - i, "%s", attrs & OBJ_ATTR_REFERENCE ? "*" : "-");

        i += cpyf(&buffer[i], BUFFER_SIZE - i, " ");

        int64_t type = object->objattrs & OBJ_TYPE_MASK;
        switch (type) {
                case OBJ_TYPE_NUMBER:
                        i += cpyf(&buffer[i], BUFFER_SIZE - i, "static_int");
                        break;
                
                case OBJ_TYPE_SEQUENCE:
                        i += cpyf(&buffer[i], BUFFER_SIZE - i, "sequence");
                        break;
                
                case OBJ_TYPE_BUFFER:
                        i += cpyf(&buffer[i], BUFFER_SIZE - i, "u8_buf");
                        break;
                
                case OBJ_TYPE_STRING:
                        i += cpyf(&buffer[i], BUFFER_SIZE - i, "static_str");
                        break;
                
                case OBJ_TYPE_UNDEFINED:
                        i += cpyf(&buffer[i], BUFFER_SIZE - i, "undefined");
                        break;
        }
        return (strdup(buffer));
}
