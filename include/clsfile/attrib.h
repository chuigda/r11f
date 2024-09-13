#ifndef R11F_CLASSFILE_ATTRIBUTE_H
#define R11F_CLASSFILE_ATTRIBUTE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t info[];
} r11f_attribute_info_t;

typedef struct {
    uint16_t start_pc;
    uint16_t end_pc;
    uint16_t handler_pc;
    uint16_t catch_type;
} r11f_exception_table;

typedef struct {
    uint16_t max_stacks;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t *code;
} r11f_code_attribute;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSFILE_ATTRIBUTE_H */
