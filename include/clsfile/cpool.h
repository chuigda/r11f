#ifndef R11F_CLASSFILE_CONSTANT_POOL_H
#define R11F_CLASSFILE_CONSTANT_POOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    R11F_CONSTANT_Class = 7,
    R11F_CONSTANT_Fieldref = 9,
    R11F_CONSTANT_Methodref = 10,
    R11F_CONSTANT_InterfaceMethodref = 11,
    R11F_CONSTANT_String = 8,
    R11F_CONSTANT_Integer = 3,
    R11F_CONSTANT_Float = 4,
    R11F_CONSTANT_Long = 5,
    R11F_CONSTANT_Double = 6,
    R11F_CONSTANT_NameAndType = 12,
    R11F_CONSTANT_Utf8 = 1,
    R11F_CONSTANT_MethodHandle = 15,
    R11F_CONSTANT_MethodType = 16,
    R11F_CONSTANT_InvokeDynamic = 18
};

typedef struct {
    uint8_t tag;
    uint8_t info[];
} r11f_cpinfo_t;

typedef struct {
    uint8_t tag;
    uint16_t name_index;
} r11f_constant_class_info_t;

typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} r11f_constant_fieldref_info_t;

typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} r11f_constant_methodref_info_t;

typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} r11f_constant_interface_methodref_info_t;

typedef struct {
    uint8_t tag;
    uint16_t string_index;
} r11f_constant_string_info_t;

typedef struct {
    uint8_t tag;
    uint32_t bytes;
} r11f_constant_integer_info_t;

typedef struct {
    uint8_t tag;
    uint32_t bytes;
} r11f_constant_float_info_t;

typedef struct {
    uint8_t tag;
    uint32_t high_bytes;
    uint32_t low_bytes;
} r11f_constant_long_info_t;

typedef struct {
    uint8_t tag;
    uint32_t high_bytes;
    uint32_t low_bytes;
} r11f_constant_double_info_t;

typedef struct {
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
} r11f_constant_name_and_type_info_t;

typedef struct {
    uint8_t tag;
    uint16_t length;
    uint8_t bytes[];
} r11f_constant_utf8_info_t;

typedef struct {
    uint8_t tag;
    uint8_t reference_kind;
    uint16_t reference_index;
} r11f_constant_method_handle_info_t;

typedef struct {
    uint8_t tag;
    uint16_t descriptor_index;
} r11f_constant_method_type_info_t;

typedef struct {
    uint8_t tag;
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
} r11f_constant_invoke_dynamic_info_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSFILE_CONSTANT_POOL_H */