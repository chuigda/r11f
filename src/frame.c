#include "frame.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "alloc.h"
#include "byteutil.h"
#include "class.h"
#include "class/attrib.h"

R11F_EXPORT r11f_frame_t*
r11f_frame_alloc(r11f_class_t *clazz, r11f_method_info_t *method_info) {
    r11f_attribute_info_t *code_info =
        r11f_method_find_attribute(clazz, method_info, "Code");
    assert(code_info && "attribute Code not found in method"
                        ", is this method abstract or native?");

    uint16_t max_stack = read_unaligned2(code_info->info);
    uint16_t max_locals = read_unaligned2(code_info->info + 2);
    uint16_t code_length = read_unaligned2(code_info->info + 4);
    uint8_t *code = code_info->info + 8;

    size_t vla_size = (max_stack + max_locals) * sizeof(r11f_value_t);
    r11f_frame_t *frame = r11f_alloc(sizeof(r11f_frame_t) + vla_size);
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

    frame->stack = (r11f_value_t*)(frame->data);
    frame->locals = (r11f_value_t*)(frame->data + max_stack);
    return frame;
}
