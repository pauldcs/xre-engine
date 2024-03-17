#ifndef __XRE_OPERATIONS_H__
#define __XRE_OPERATIONS_H__

#include "xre_runtime.h"
#include <stdbool.h>

bool arithmetic_op(xre_frame_t *frame);
bool relational_op(xre_frame_t *frame);
bool bitwise_op(xre_frame_t *frame);
bool loop_op(xre_frame_t *frame);
bool logical_op(xre_frame_t *frame);
bool assignment_op(xre_frame_t *frame);
bool not_op(xre_frame_t *frame);
bool separator_op(xre_frame_t *frame);
bool sequence_op(xre_frame_t *frame);
bool print_op(xre_frame_t *frame);
bool at_op(xre_frame_t *frame);

bool add_op(xre_frame_t *frame);
bool sub_op(xre_frame_t *frame);
bool mul_op(xre_frame_t *frame);
bool div_op(xre_frame_t *frame);
bool mod_op(xre_frame_t *frame);

bool identifier_op(xre_frame_t *frame);
bool value_op(xre_frame_t *frame);
bool string_op(xre_frame_t *frame);

#endif /* __XRE_OPERATIONS_H__ */
