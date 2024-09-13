#ifndef R11F_CLASSFILE_H
#define R11F_CLASSFILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "defs.h"

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

typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t info[];
} r11f_attribute_info_t;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    r11f_attribute_info_t **attributes;
} r11f_field_info_t;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    r11f_attribute_info_t **attributes;
} r11f_method_info_t;

typedef struct {
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
} r11f_classfile_t;

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

R11F_EXPORT bool r11f_classfile_read(FILE *file, r11f_classfile_t *classfile);
R11F_EXPORT void r11f_classfile_cleanup(r11f_classfile_t *classfile);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSFILE_H */
