#include "xre_frame.h"
#include "xre_runtime.h"
#include "xre_inject.h"
#include <stdbool.h>

bool inject_write(xre_frame_t *frame) {
    state_print(frame);
    return (true);
}
