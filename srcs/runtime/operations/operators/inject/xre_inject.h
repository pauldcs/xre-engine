#ifndef __XRE_INJECT_H__
# define __XRE_INJECT_H__

# include "xre_frame.h"

typedef struct {
    const char *name;
    bool      (*func)(xre_frame_t *);
} injectee_t;

bool inject_write(xre_frame_t *frame);
bool inject_at(xre_frame_t *frame);

#endif /* __XRE_INJECT_H__ */
