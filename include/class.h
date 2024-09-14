#ifndef R11F_CLASS_H
#define R11F_CLASS_H

#include <stdint.h>

#include "class/cpool.h"
#include "defs.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    r11f_attribute_info_t **attributes;
} r11f_field_info_t;

typedef struct st_r11f_method_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    r11f_attribute_info_t **attributes;
} r11f_method_info_t;

typedef struct st_r11f_class {
    uint32_t magic;
    uint16_t major_version;
    uint16_t minor_version;
    uint16_t constant_pool_count;
    void **constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    uint16_t *interfaces;
    uint16_t fields_count;
    r11f_field_info_t **fields;
    uint16_t methods_count;
    r11f_method_info_t **methods;
    uint16_t attributes_count;
    r11f_attribute_info_t **attributes;
} r11f_class_t;

enum {
    R11F_ACC_PUBLIC = 0x0001,
    R11F_ACC_PRIVATE = 0x0002,
    R11F_ACC_PROTECTED = 0x0004,
    R11F_ACC_STATIC = 0x0008,
    R11F_ACC_FINAL = 0x0010,
    R11F_ACC_SUPER = 0x0020,
    R11F_ACC_SYNCHRONIZED = 0x0020,
    R11F_ACC_BRIDGE = 0x0040,
    R11F_ACC_VARARGS = 0x0080,
    R11F_ACC_NATIVE = 0x0100,
    R11F_ACC_INTERFACE = 0x0200,
    R11F_ACC_ABSTRACT = 0x0400,
    R11F_ACC_SYNTHETIC = 0x1000,
    R11F_ACC_ANNOTATION = 0x2000,
    R11F_ACC_ENUM = 0x4000,
};

R11F_EXPORT void r11f_class_cleanup(r11f_class_t *clazz);

R11F_EXPORT r11f_method_info_t*
r11f_class_resolve_method(r11f_class_t *clazz,
                          char const *name,
                          uint16_t name_len,
                          char const *descriptor,
                          uint16_t descriptor_len);

typedef struct {
    char const* name;
    uint16_t name_len;
    char const *descriptor;
    uint16_t descriptor_len;
} r11f_method_qual_name_t;

R11F_EXPORT r11f_method_qual_name_t
r11f_class_get_method_name(r11f_class_t *clazz,
                           r11f_constant_methodref_info_t *methodref_info);

R11F_EXPORT r11f_method_info_t*
r11f_class_resolve_method2(r11f_class_t *clazz,
                           r11f_constant_methodref_info_t *methodref_info);

R11F_EXPORT r11f_attribute_info_t*
r11f_method_find_attribute(r11f_class_t *clazz,
                           r11f_method_info_t *method_info,
                           char const *name);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASS_H */
