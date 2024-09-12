#ifndef R11F_CLASSFILE_H
#define R11F_CLASSFILE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t tag;
    uint8_t info[0];
} r11f_cpinfo_t;

typedef struct {
    uint32_t magic;
    uint16_t major_version;
    uint16_t minor_version;
    uint16_t constant_pool_count;
    r11f_cpinfo_t **constant_pool;
} r11f_classfile_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSFILE_H */
