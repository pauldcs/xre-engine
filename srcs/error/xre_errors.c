#include "xre_errors.h"

const char *err_type_to_string(t_xre_error_type type) {
    switch (type) {
        case XRE_MEMORY_ERROR:      return "Memory Error";
        case XRE_ARITHMETIC_ERROR:  return "Arithmetic Error";
        case XRE_IMPORT_ERROR:      return "Import Error";
        case XRE_LOOKUP_ERROR:      return "Lookup Error";
        case XRE_INTERNAL_ERROR:    return "Internal Error";
        case XRE_INDENTIFIER_ERROR: return "Unknown Identifier Error";
        case XRE_ACCESS_VIOLATION:  return "Access Error";
        case XRE_EXIT_ERROR:        return "Exit Error";
        case XRE_OS_ERROR:          return "OS Error";
        case XRE_RUNTIME_ERROR:     return "Runtime error";
        case XRE_SYNTAX_ERROR:      return "Syntax Error";
        case XRE_TYPE_ERROR:        return "Type Error";
        case XRE_VALUE_ERROR:       return "Value Error";
    }
    return "Unknown Error";
}

const char *err_subtype_to_string(t_xre_error_subtype type) {
    switch (type) {
        case XRE_MODULE_NOT_FOUND_ERROR:  return "The required module was not found";
        case XRE_ALREADY_ANNOTATED_ERROR: return "Already annotated";
        case XRE_INVALID_MODULE_ERROR:    return "The required module is invalid";
        case XRE_CONFUSING_CONDITION:     return "A confusing internal condition happended";
        case XRE_ASSERTION_ERROR:         return "Failed an assertion check in the code";
        case XRE_MODE_WRITE_ON_STRICT: return "Attempt to write on a protected variable";
        case XRE_WRITE_ON_READONLY:    return "Attempt to write on a readonly variable";
        case XRE_INVALID_ANNOTATION_ERROR: return "Attempt to annotate a variable with an invalid type";
        case XRE_UNKNOWN_ANNOTATION_ERROR: return "Unknown annotation";
        case XRE_TYPE_MISSMATCH_ERROR:    return "Missmatching type";
        case XRE_EXIT_CALLED_ERROR:       return "Exit called";
        case XRE_INVALID_SCOPE_RESOLUTION: return "Invalid scope resolution";
        case XRE_UNKNOWN_COMMAND_ERROR:    return "Unknown command";
        case XRE_NEGATIVE_SHIFT_ERROR:    return "Shift value cannot be negative";
        case XRE_EXCEEDS_SHIFT_LIMIT_ERROR: return "Exceeds the maximum shift count";
        case XRE_INVALID_TYPE_FOR_OPERAND: return "Operator unsupported for this type";
        case XRE_ALLOCATION_ERROR:        return "Failed to allocate memory";
        case XRE_INDEX_ERROR:             return "Indexing an out of bounds element";
        case XRE_UNBOUND_LOCAL_ERROR:     return "Unbound variable";
        case XRE_UNINITIALIZED_LOCAL_ERROR:  return "Accessing an uninitialized local variable";
        case XRE_NOT_IMPLEMENTED_ERROR:   return "Not implemented";
        case XRE_NOT_TERMINATED_ERROR:    return "Terminated";
        case XRE_OVERFLOW_ERROR:          return "A numeric overflow occurred";
        case XRE_UNDERFLOW_ERROR:         return "A numeric underflow occurred";
        case XRE_ZERO_DIVISION_ERROR:     return "Attempted a division by zero";
        case XRE_BROKEN_PIPE_ERROR:       return "A pipe communication failed";
        case XRE_FILE_EXISTS_ERROR:       return "A file with the same name already exists";
        case XRE_FILE_NOT_FOUND_ERROR:    return "The specified file was not found";
        case XRE_IS_A_DIRECTORY_ERROR:    return "Got an unexpected directory";
        case XRE_NOT_A_DIRECTORY_ERROR:   return "Expected a directory";
        case XRE_PERMISSION_ERROR:        return "Insufficient permissions to perform the operation";
        case XRE_INTERRUPTED_ERROR:       return "The operation was interrupted";
        case XRE_TIMEOUT_ERROR:           return "The operation exceeded the allowed time limit";
        case XRE_IO_ERROR:                return "An input/output operation failed";
        case XRE_INVALID_ASSIGMENT_ERROR: return "Invalid assignement";
        case XRE_UNEXPECTED_OPERATOR:     return "Unexpected operator";
        case XRE_UNEXPECTED_OPERAND:      return "Unexpected operand";
        case XRE_UNMATCHED_PARENTHESIS:   return "Unmatched parenthesis";
        case XRE_UNTERMINATED_STRING_ERROR: return "the string in unterminated";
    }
    return "No information";
}
