#include "frame.h"

#include <stdlib.h>
#include "byteutil.h"
#include "clsfile.h"

R11F_EXPORT
r11f_frame_t *r11f_frame_alloc(uint16_t max_locals, uint16_t max_stack) {
    r11f_frame_t *frame = malloc(
        sizeof(r11f_frame_t) + (max_locals + max_stack) * sizeof(uint32_t)
    );
    if (!frame) {
        return NULL;
    }

    frame->parent = NULL;
    frame->constant_pool_count = 0;
    frame->constant_pool = NULL;
    frame->max_locals = max_locals;
    frame->max_stack = max_stack;
    frame->sp = 0;
    frame->locals = frame->data;
    frame->stack = frame->data + max_locals;

    return frame;
}

R11F_EXPORT void
r11f_frame_setref(r11f_frame_t *frame, r11f_classfile_t *classfile) {
    frame->constant_pool_count = classfile->constant_pool_count;
    frame->constant_pool = classfile->constant_pool;
}


R11F_EXPORT int64_t r11f_frame_get_int64(r11f_frame_t *frame, uint16_t entry) {
    if (entry % 2 == 0) {
        return *(int64_t*)(frame->data + entry);
    }
    else {
        uint64_t r = read_unaligned8(frame->data + entry);
        return *(int64_t*)&r;
    }
}

R11F_EXPORT void
r11f_frame_set_int64(r11f_frame_t *frame, uint16_t entry, int64_t value) {
    if (entry % 2 == 0) {
        *(int64_t*)(frame->data + entry) = value;
    }
    else {
        write_unaligned8(frame->data + entry, *(uint64_t*)&value);
    }
}

R11F_EXPORT double r11f_frame_get_double(r11f_frame_t *frame, uint16_t entry) {
    if (entry % 2 == 0) {
        return *(double*)(frame->data + entry);
    }
    else {
        uint64_t r = read_unaligned8(frame->data + entry);
        return *(double*)&r;
    }
}

R11F_EXPORT void
r11f_frame_set_double(r11f_frame_t *frame, uint16_t entry, double value) {
    if (entry % 2 == 0) {
        *(double*)(frame->data + entry) = value;
    }
    else {
        write_unaligned8(frame->data + entry, *(uint64_t*)&value);
    }
}
