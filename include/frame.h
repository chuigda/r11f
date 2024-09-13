#ifndef R11F_FRAME_H
#define R11F_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include "defs.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

struct st_r11f_frame {
    r11f_frame_t *parent;

    uint16_t constant_pool_count;
    void **constant_pool;

    uint16_t max_locals;
    uint16_t max_stack;
    uint16_t sp;

    uint32_t *locals;
    uint32_t *stack;
    uint32_t data[];
};

R11F_EXPORT
r11f_frame_t *r11f_frame_alloc(uint16_t max_locals, uint16_t max_stack);

R11F_EXPORT void
r11f_frame_setref(r11f_frame_t *frame, r11f_classfile_t *classfile);

R11F_EXPORT int64_t r11f_frame_get_int64(r11f_frame_t *frame, uint16_t entry);
R11F_EXPORT void
r11f_frame_set_int64(r11f_frame_t *frame, uint16_t entry, int64_t value);

R11F_EXPORT double r11f_frame_get_double(r11f_frame_t *frame, uint16_t entry);
R11F_EXPORT void
r11f_frame_set_double(r11f_frame_t *frame, uint16_t entry, double value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_FRAME_H */
