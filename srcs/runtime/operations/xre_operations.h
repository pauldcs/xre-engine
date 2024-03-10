#ifndef __XRE_OPERATIONS_H__
# define __XRE_OPERATIONS_H__

# include "xre_frame.h"

bool arithmetic_op(xre_frame_t *frame);
bool relational_op(xre_frame_t *frame);
bool bitwise_op(xre_frame_t *frame);
bool loop_op(xre_frame_t *frame);
bool logical_op(xre_frame_t *frame);
bool assignment_op(xre_frame_t *frame);
bool not_op(xre_frame_t *frame);
bool sequence_op(xre_frame_t *frame);
bool inject_op(xre_frame_t *frame);
bool separator_op(xre_frame_t *frame);
bool annotation_op(xre_frame_t *frame);
bool scope_resolution_op(xre_frame_t *frame);

bool add_op(xre_frame_t *frame);
bool sub_op(xre_frame_t *frame);
bool mul_op(xre_frame_t *frame);
bool div_op(xre_frame_t *frame);
bool mod_op(xre_frame_t *frame);

bool identifier_operand(xre_frame_t *frame);
bool basic_operand(xre_frame_t *frame);
bool array_operand(xre_frame_t *frame);

#endif /* __XRE_OPERATIONS_H__ */
