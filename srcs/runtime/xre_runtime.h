#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_frame.h"
#include "xre_parse.h"
#include "xre_log.h"
#include <stdbool.h>

#define DEFAULT_MAX_ITERATIONS 5000
#define DEFAULT_FRAME_SIZE 64

# if 0
#define log_error_condition_reached  XRE_LOGGER(debug, "error state reached")
#define log_error_return             XRE_LOGGER(debug, "error returned")
#define log_error_details(string)            XRE_LOGGER(debug, " > %s", string)
# else
#define log_error_condition_reached 
#define log_error_return           
#define log_error_details(string) 
# endif

bool xre_runtime(xre_ast_t *ast);
bool evaluate(xre_frame_t *frame);

#endif /* __XRE_RUNTIME_H__ */
