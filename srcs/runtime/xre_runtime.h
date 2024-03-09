#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_frame.h"
#include "xre_parse.h"
#include <stdbool.h>

#define DEFAULT_MAX_ITERATIONS 5000
#define DEFAULT_FRAME_SIZE 64

bool xre_runtime(xre_ast_t *ast);
bool evaluate(xre_frame_t *frame);

#endif /* __XRE_RUNTIME_H__ */
