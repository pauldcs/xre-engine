#include "xre_errors.h"

error_class_e error_type_to_class(error_type_e type)
{
	switch (type) {
	case XRE_ALREADY_ANNOTATED_ERROR:
	case XRE_MODE_WRITE_ON_STRICT_ERROR:
	case XRE_WRITE_ON_READONLY_ERROR:
		return XRE_ACCESS_ERROR_C;

	case XRE_UNDEFINED_BEHAVIOR_ERROR:
	case XRE_NOT_IMPLEMENTED_ERROR:
		return XRE_INTERNAL_ERROR_C;

	case XRE_OUT_OF_MEMORY_ERROR:
	case XRE_STACK_OVERFLOW_ERROR:
		return XRE_MEMORY_ERROR_C;

	case XRE_INVALID_ANNOTATION_ERROR:
	case XRE_TYPE_MISSMATCH_ERROR:
	case XRE_INVALID_TYPE_FOR_OPERAND_ERROR:
	case XRE_INVALID_ASSIGMENT_ERROR:
	case XRE_UNREADABLE_ERROR:
		return XRE_TYPE_ERROR_C;

	case XRE_UNKNOWN_ANNOTATION_ERROR:
	case XRE_UNKNOWN_COMMAND_ERROR:
	case XRE_UNBOUND_LOCAL_ERROR:
	case XRE_UNDEFINED_BUILTIN_ERROR:
		return XRE_NAME_ERROR_C;

	case XRE_MAX_ITERATIONS_ERROR:
		return XRE_TERMINATION_ERROR_C;

	case XRE_OUT_OF_BOUNDS_ACCESS_ERROR:
		return XRE_INDEX_ERROR_C;

	case XRE_NEGATIVE_SHIFT_ERROR:
	case XRE_EXCEEDS_SHIFT_LIMIT_ERROR:
	case XRE_OVERFLOW_ERROR:
	case XRE_ZERO_DIVISION_ERROR:
	case XRE_UNDERFLOW_ERROR:
		return XRE_ARITHMETIC_ERROR_C;

	case XRE_UNEXPECTED_OPERATOR_ERROR:
	case XRE_ARGUMENTS_ERROR:
	case XRE_UNEXPECTED_OPERAND_ERROR:
	case XRE_UNMATCHED_PARENTHESIS_ERROR:
	case XRE_UNTERMINATED_STRING_ERROR:
		return XRE_SYNTAX_ERROR_C;
	}

	__builtin_unreachable();
}

const char *error_class_str(error_class_e class)
{
	switch (class) {
	case XRE_MEMORY_ERROR_C:
		return "Memory Error";
	case XRE_ARITHMETIC_ERROR_C:
		return "Arithmetic Error";
	case XRE_INTERNAL_ERROR_C:
		return "Internal Error";
	case XRE_INDEX_ERROR_C:
		return "Index Error";
	case XRE_NAME_ERROR_C:
		return "Name Error";
	case XRE_ACCESS_ERROR_C:
		return "Access Error";
	case XRE_TERMINATION_ERROR_C:
		return "Termination Error";
	case XRE_RUNTIME_ERROR_C:
		return "Runtime error";
	case XRE_SYNTAX_ERROR_C:
		return "Syntax Error";
	case XRE_TYPE_ERROR_C:
		return "Type Error";
	case XRE_VALUE_ERROR_C:
		return "Value Error";
	}

	__builtin_unreachable();
}

const char *error_type_str(error_type_e type)
{
	switch (type) {
	case XRE_ALREADY_ANNOTATED_ERROR:
		return "already annotated";
	case XRE_UNDEFINED_BEHAVIOR_ERROR:
		return "undefined behavior happened";
	case XRE_OUT_OF_MEMORY_ERROR:
		return "out of memory";
	case XRE_STACK_OVERFLOW_ERROR:
		return "stack overflow";
	case XRE_MAX_ITERATIONS_ERROR:
		return "max iterations reached";
	case XRE_MODE_WRITE_ON_STRICT_ERROR:
		return "attempt to write on a protected variable";
	case XRE_UNREADABLE_ERROR:
		return "read protected value";
	case XRE_ARGUMENTS_ERROR:
		return "invalid arguments";
	case XRE_WRITE_ON_READONLY_ERROR:
		return "attempt to write on a readonly variable";
	case XRE_INVALID_ANNOTATION_ERROR:
		return "attempt to annotate a variable with an invalid type";
	case XRE_UNKNOWN_ANNOTATION_ERROR:
		return "unknown annotation";
	case XRE_TYPE_MISSMATCH_ERROR:
		return "missmatching types";
	case XRE_UNKNOWN_COMMAND_ERROR:
		return "unknown command";
	case XRE_NEGATIVE_SHIFT_ERROR:
		return "shift value cannot be negative";
	case XRE_EXCEEDS_SHIFT_LIMIT_ERROR:
		return "exceeds the maximum shift count";
	case XRE_INVALID_TYPE_FOR_OPERAND_ERROR:
		return "operator unsupported for this type";
	case XRE_UNBOUND_LOCAL_ERROR:
		return "unbound local variable";
	case XRE_UNDEFINED_BUILTIN_ERROR:
		return "undefined builtin";
	case XRE_OUT_OF_BOUNDS_ACCESS_ERROR:
		return "out of bounds access";
	case XRE_NOT_IMPLEMENTED_ERROR:
		return "not implemented";
	case XRE_OVERFLOW_ERROR:
		return "a numeric overflow occurred";
	case XRE_UNDERFLOW_ERROR:
		return "a numeric underflow occurred";
	case XRE_ZERO_DIVISION_ERROR:
		return "attempted a division by zero";
	case XRE_INVALID_ASSIGMENT_ERROR:
		return "invalid assignement";
	case XRE_UNEXPECTED_OPERATOR_ERROR:
		return "unexpected operator";
	case XRE_UNEXPECTED_OPERAND_ERROR:
		return "unexpected operand";
	case XRE_UNMATCHED_PARENTHESIS_ERROR:
		return "unmatched parenthesis";
	case XRE_UNTERMINATED_STRING_ERROR:
		return "unterminated string";
	}

	__builtin_unreachable();
}
