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

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSFILE_ATTRIBUTE_H */
