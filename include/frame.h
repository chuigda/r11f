#ifndef R11F_FRAME_H
#define R11F_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include "defs.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union u_r11f_value {
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float f32;
    double f64;

    uint32_t u32;
    void *ptr;
} r11f_value_t;

struct st_r11f_frame {
    r11f_frame_t *parent;

    r11f_class_t *clazz;
    r11f_method_info_t *method_info;

    uint32_t pc;
    uint32_t code_length;
    uint8_t *code;

    uint16_t max_locals;
    uint16_t max_stack;
    uint16_t sp;

    r11f_value_t *stack;
    r11f_value_t *locals;

    r11f_value_t data[];
};

R11F_EXPORT r11f_frame_t*
r11f_frame_alloc(r11f_class_t *clazz, r11f_method_info_t *method_info);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_FRAME_H */
