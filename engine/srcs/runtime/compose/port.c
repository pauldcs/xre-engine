#include "xre_runtime.h"
#include "defaults.h"

const char *port_type_string(enum memory_type type)
{
	switch (type) {
	case I64:
		return ("i64");
	case VEC_OBJECT:
		return ("vec<object_t>");
	case VEC_U8:
		return ("vec<u8>");
	case STRING:
		return ("string");
	case BUFFER:
		return ("buffer");
	case UNDEFINED:
		return ("any");
	case TYPE_NONE:
		return ("type none");
	};
}

const char *port_prot_string(enum memory_prot prot)
{
	switch (prot) {
	case RD:
		return ("read");
	case WR:
		return ("mutate");
	case RDWR:
		return ("unrestricted");
	case PROT_NONE:
		return ("restricted");
	};
}

struct port default_number_port(void)
{
	static struct port port = __number_port;
	return (port);
}

struct port default_undefined_port(void)
{
	static struct port port = __undefined_port;
	return (port);
}

struct port default_string_port(void)
{
	static struct port port = __readable_string_port;
	return (port);
}

struct port default_sequence_port(void)
{
	static struct port port = __readable_sequence_port;
	return (port);
}
