#include "frame.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "byteutil.h"
#include "class.h"
#include "class/attrib.h"
#include "class/cpool.h"

R11F_EXPORT r11f_frame_t*
r11f_frame_alloc(r11f_class_t *clazz, r11f_method_info_t *method_info) {
    r11f_attribute_info_t *code_info = NULL;
    for (uint16_t i = 0; i < method_info->attributes_count; i++) {
        r11f_attribute_info_t *attr = method_info->attributes[i];
        r11f_constant_utf8_info_t *name_info =
            clazz->constant_pool[attr->attribute_name_index];
        if (!strncmp("Code", (char*)name_info->bytes, name_info->length)) {
            code_info = attr;
            break;
        }
    }
    assert(code_info && "attribute Code not found in method"
                        ", is this method abstract or native?");

    uint16_t max_stack = read_unaligned2(code_info->info);
    uint16_t max_locals = read_unaligned2(code_info->info + 2);
    uint16_t code_length = read_unaligned2(code_info->info + 4);
    uint8_t *code = code_info->info + 8;

    r11f_frame_t *frame = malloc(
        sizeof(r11f_frame_t) + (max_locals + max_stack) * sizeof(uint32_t)
    );
    if (!frame) {
        return NULL;
    }

    frame->parent = NULL;
    frame->clazz = clazz;
    frame->method_info = method_info;
    frame->pc = 0;
    frame->code_length = code_length;
    frame->code = code;
    frame->max_locals = max_locals;
    frame->max_stack = max_stack;
    frame->sp = 0;
    frame->locals = frame->data;
    frame->stack = frame->data + max_locals;

    return frame;
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
