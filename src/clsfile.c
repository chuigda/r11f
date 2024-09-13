#include "clsfile.h"

#include <error.h>
#include <inttypes.h>
#include <stdlib.h>
#include "class.h"
#include "class/cpool.h"
#include "class/attrib.h"
#include "fileutil.h"

#ifdef R11F_LITTLE_ENDIAN
#include <string.h>
#include "byteutil.h"
#endif

#define CHKERR_RET(expr) \
    { \
        r11f_error_t errtemp = (expr); \
        if (errtemp != R11F_success) { \
            return errtemp; \
        } \
    }

#define CHKFALSE_RET(expr, ret) \
    { \
        if (!(expr)) { \
            return ret; \
        } \
    }

#define CHKREAD(readfn, file, value) \
    if (!readfn(file, value)) { \
        return R11F_ERR_malformed_classfile; \
    }

#define CHKREADBYTES(file, buffer, size) \
    if (!read_byte_array(file, buffer, size)) { \
        return R11F_ERR_malformed_classfile; \
    }

static r11f_error_t read_header(FILE *file, r11f_class_t *clazz);
static r11f_error_t read_constant_pool(FILE *file, r11f_class_t *clazz);
static r11f_error_t read_classinfo(FILE *file, r11f_class_t *clazz);
static r11f_error_t read_interfaces(FILE *file, r11f_class_t *clazz);
static r11f_error_t read_fields(FILE *file, r11f_class_t *clazz);
static r11f_error_t read_methods(FILE *file, r11f_class_t *clazz);
static r11f_error_t read_attributes(FILE *file, r11f_class_t *clazz);
static r11f_error_t imp_read_attributes(FILE *file,
                                        size_t n,
                                        r11f_attribute_info_t **attributes,
                                        r11f_class_t *clazz);

#ifdef R11F_LITTLE_ENDIAN
static void preprocess_code_attribute(r11f_attribute_info_t *attribute);
#endif

R11F_EXPORT r11f_error_t r11f_classfile_read(FILE *file, r11f_class_t *clazz) {
    CHKERR_RET(read_header(file, clazz))
    CHKERR_RET(read_constant_pool(file, clazz))
    CHKERR_RET(read_classinfo(file, clazz))
    CHKERR_RET(read_interfaces(file, clazz))
    CHKERR_RET(read_fields(file, clazz))
    CHKERR_RET(read_methods(file, clazz))
    CHKERR_RET(read_attributes(file, clazz))

    return R11F_success;
}

static r11f_error_t read_header(FILE *file, r11f_class_t *clazz) {
    if (!read_u4(file, &clazz->magic)
        || clazz->magic != 0xCAFEBABE) {
        return R11F_ERR_malformed_classfile;
    }

    if (!read_u2(file, &clazz->minor_version)
        || clazz->minor_version != 0) {
        return R11F_ERR_malformed_classfile;
    }

    if (!read_u2(file, &clazz->major_version)
        || clazz->major_version != 52) {
        return R11F_ERR_malformed_classfile;
    }

    return R11F_success;
}

static r11f_error_t
read_constant_pool(FILE *file, r11f_class_t *clazz) {
    CHKREAD(read_u2, file, &clazz->constant_pool_count)
    CHKFALSE_RET(clazz->constant_pool = calloc(
        clazz->constant_pool_count,
        sizeof(r11f_cpinfo_t *)
    ), R11F_ERR_out_of_memory)

    for (uint16_t i = 1; i < clazz->constant_pool_count; i++) {
        uint8_t tag;
        CHKREAD(read_byte, file, &tag)

        size_t size;
        switch (tag) {
            case R11F_CONSTANT_Class:
                size = sizeof(r11f_constant_class_info_t);
                break;
            case R11F_CONSTANT_Fieldref:
                size = sizeof(r11f_constant_fieldref_info_t);
                break;
            case R11F_CONSTANT_Methodref:
                size = sizeof(r11f_constant_methodref_info_t);
                break;
            case R11F_CONSTANT_InterfaceMethodref:
                size = sizeof(r11f_constant_interface_methodref_info_t);
                break;
            case R11F_CONSTANT_String:
                size = sizeof(r11f_constant_string_info_t);
                break;
            case R11F_CONSTANT_Integer:
                size = sizeof(r11f_constant_integer_info_t);
                break;
            case R11F_CONSTANT_Float:
                size = sizeof(r11f_constant_integer_info_t);
                break;
            case R11F_CONSTANT_Long:
                size = sizeof(r11f_constant_integer_info_t);
                break;
            case R11F_CONSTANT_Double:
                size = sizeof(r11f_constant_integer_info_t);
                break;
            case R11F_CONSTANT_NameAndType:
                size = sizeof(r11f_constant_name_and_type_info_t);
                break;
            case R11F_CONSTANT_Utf8:
                /* defer allocation until we know the length */
                size = 0;
                break;
            case R11F_CONSTANT_MethodHandle:
                size = sizeof(r11f_constant_method_handle_info_t);
                break;
            case R11F_CONSTANT_MethodType:
                size = sizeof(r11f_constant_method_type_info_t);
                break;
            case R11F_CONSTANT_InvokeDynamic:
                size = sizeof(r11f_constant_invoke_dynamic_info_t);
                break;
            default:
                return R11F_ERR_malformed_classfile;
        }

        if (size) {
            CHKFALSE_RET(clazz->constant_pool[i] = malloc(size),
                         R11F_ERR_out_of_memory)
            ((r11f_cpinfo_t*)clazz->constant_pool[i])->tag = tag;
        }

        switch (tag) {
            case R11F_CONSTANT_Class: {
                r11f_constant_class_info_t *class_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u2, file, &class_info->name_index)
                break;
            }
            case R11F_CONSTANT_Fieldref: {
                r11f_constant_fieldref_info_t *fieldref_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u2, file, &fieldref_info->class_index)
                CHKREAD(read_u2, file, &fieldref_info->name_and_type_index)
                break;
            }
            case R11F_CONSTANT_Methodref: {
                r11f_constant_methodref_info_t *methodref_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u2, file, &methodref_info->class_index)
                CHKREAD(read_u2, file, &methodref_info->name_and_type_index)
                break;
            }
            case R11F_CONSTANT_InterfaceMethodref: {
                r11f_constant_interface_methodref_info_t *interface_methodref_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u2, file, &interface_methodref_info->class_index)
                CHKREAD(
                    read_u2,
                    file,
                    &interface_methodref_info->name_and_type_index
                )
                break;
            }
            case R11F_CONSTANT_String: {
                r11f_constant_string_info_t *string_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u2, file, &string_info->string_index)
                break;
            }
            case R11F_CONSTANT_Integer: {
                r11f_constant_integer_info_t *integer_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u4, file, &integer_info->bytes)
                break;
            }
            case R11F_CONSTANT_Float: {
                r11f_constant_float_info_t *float_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u4, file, &float_info->bytes)
                break;
            }
            case R11F_CONSTANT_Long: {
                r11f_constant_long_info_t *long_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u4, file, &long_info->high_bytes)
                CHKREAD(read_u4, file, &long_info->low_bytes)
                i++;
                break;
            }
            case R11F_CONSTANT_Double: {
                r11f_constant_double_info_t *double_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u4, file, &double_info->high_bytes)
                CHKREAD(read_u4, file, &double_info->low_bytes)
                i++;
                break;
            }
            case R11F_CONSTANT_NameAndType: {
                r11f_constant_name_and_type_info_t *name_and_type_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u2, file, &name_and_type_info->name_index)
                CHKREAD(read_u2, file, &name_and_type_info->descriptor_index)
                break;
            }
            case R11F_CONSTANT_Utf8: {
                uint16_t length;
                CHKREAD(read_u2, file, &length)

                r11f_constant_utf8_info_t *utf8_info = malloc(
                    sizeof(r11f_constant_utf8_info_t) + length
                );
                CHKFALSE_RET(clazz->constant_pool[i] = utf8_info,
                             R11F_ERR_out_of_memory)

                utf8_info->tag = R11F_CONSTANT_Utf8;
                utf8_info->length = length;
                CHKREADBYTES(file, utf8_info->bytes, length)
                break;
            }
            case R11F_CONSTANT_MethodHandle: {
                r11f_constant_method_handle_info_t *method_handle_info =
                    clazz->constant_pool[i];
                CHKREAD(read_byte, file, &method_handle_info->reference_kind)
                CHKREAD(read_u2, file, &method_handle_info->reference_index)
                break;
            }
            case R11F_CONSTANT_MethodType: {
                r11f_constant_method_type_info_t *method_type_info =
                    clazz->constant_pool[i];
                CHKREAD(read_u2, file, &method_type_info->descriptor_index)
                break;
            }
            case R11F_CONSTANT_InvokeDynamic: {
                r11f_constant_invoke_dynamic_info_t *invoke_dynamic_info =
                    clazz->constant_pool[i];
                CHKREAD(
                    read_u2,
                    file,
                    &invoke_dynamic_info->bootstrap_method_attr_index
                )
                CHKREAD(
                    read_u2,
                    file,
                    &invoke_dynamic_info->name_and_type_index
                )
                break;
            }
        }
    }

    return R11F_success;
}

static r11f_error_t
read_classinfo(FILE *file, r11f_class_t *clazz) {
    CHKREAD(read_u2, file, &clazz->access_flags)
    CHKREAD(read_u2, file, &clazz->this_class)
    CHKREAD(read_u2, file, &clazz->super_class)

    return R11F_success;
}

static r11f_error_t
read_interfaces(FILE *file, r11f_class_t *clazz) {
    CHKREAD(read_u2, file, &clazz->interfaces_count)
    CHKFALSE_RET(clazz->interfaces = malloc(
        clazz->interfaces_count * sizeof(uint16_t)
    ), R11F_ERR_out_of_memory)

    for (uint16_t i = 0; i < clazz->interfaces_count; i++) {
        CHKREAD(read_u2, file, &clazz->interfaces[i])
    }

    return R11F_success;
}

static r11f_error_t
read_fields(FILE *file, r11f_class_t *clazz) {
    CHKREAD(read_u2, file, &clazz->fields_count)
    CHKFALSE_RET(clazz->fields = malloc(
        clazz->fields_count * sizeof(r11f_field_info_t *)
    ), R11F_ERR_out_of_memory)

    for (uint16_t i = 0; i < clazz->fields_count; i++) {
        r11f_field_info_t *field_info = malloc(sizeof(r11f_field_info_t));
        CHKFALSE_RET(clazz->fields[i] = field_info,
                     R11F_ERR_out_of_memory)

        CHKREAD(read_u2, file, &field_info->access_flags)
        CHKREAD(read_u2, file, &field_info->name_index)
        CHKREAD(read_u2, file, &field_info->descriptor_index)
        CHKREAD(read_u2, file, &field_info->attributes_count)
        CHKFALSE_RET(field_info->attributes = malloc(
            field_info->attributes_count * sizeof(r11f_attribute_info_t*)
        ), R11F_ERR_out_of_memory)

        CHKERR_RET(imp_read_attributes(
            file,
            field_info->attributes_count,
            field_info->attributes,
            clazz
        ))
    }

    return R11F_success;
}

static r11f_error_t
read_methods(FILE *file, r11f_class_t *clazz) {
    CHKREAD(read_u2, file, &clazz->methods_count)
    CHKFALSE_RET(clazz->methods = malloc(
        clazz->methods_count * sizeof(r11f_method_info_t *)
    ), R11F_ERR_out_of_memory)

    for (uint16_t i = 0; i < clazz->methods_count; i++) {
        r11f_method_info_t *method_info = malloc(sizeof(r11f_method_info_t));
        CHKFALSE_RET(clazz->methods[i] = method_info,
                     R11F_ERR_out_of_memory)

        CHKREAD(read_u2, file, &method_info->access_flags)
        CHKREAD(read_u2, file, &method_info->name_index)
        CHKREAD(read_u2, file, &method_info->descriptor_index)
        CHKREAD(read_u2, file, &method_info->attributes_count)
        CHKFALSE_RET(method_info->attributes = malloc(
            method_info->attributes_count * sizeof(r11f_attribute_info_t*)
        ), R11F_ERR_out_of_memory)

        CHKERR_RET(imp_read_attributes(
            file,
            method_info->attributes_count,
            method_info->attributes,
            clazz
        ))
    }

    return R11F_success;
}

static r11f_error_t
read_attributes(FILE *file, r11f_class_t *clazz) {
    CHKREAD(read_u2, file, &clazz->attributes_count)
    CHKFALSE_RET(clazz->attributes = malloc(
        clazz->attributes_count * sizeof(r11f_attribute_info_t*)
    ), R11F_ERR_out_of_memory)

    CHKERR_RET(imp_read_attributes(
        file,
        clazz->attributes_count,
        clazz->attributes,
        clazz
    ))
    return R11F_success;
}

static r11f_error_t imp_read_attributes(FILE *file,
                                        size_t n,
                                        r11f_attribute_info_t **attributes,
                                        r11f_class_t *clazz) {
    for (size_t i = 0; i < n; i++) {
        uint16_t attribute_name_index;
        CHKREAD(read_u2, file, &attribute_name_index)
        if (attribute_name_index >= clazz->constant_pool_count) {
            return R11F_ERR_malformed_classfile;
        }
        r11f_cpinfo_t *cpinfo = clazz->constant_pool[attribute_name_index];
        if (cpinfo->tag != R11F_CONSTANT_Utf8) {
            return R11F_ERR_malformed_classfile;
        }

        uint32_t attribute_length;
        CHKREAD(read_u4, file, &attribute_length)

        r11f_attribute_info_t *attribute_info = malloc(
            sizeof(r11f_attribute_info_t) + attribute_length
        );
        CHKFALSE_RET(attributes[i] = attribute_info,
                     R11F_ERR_out_of_memory)

        attribute_info->attribute_name_index = attribute_name_index;
        attribute_info->attribute_length = attribute_length;
        CHKREADBYTES(file, attribute_info->info, attribute_length)

#ifdef R11F_LITTLE_ENDIAN
        r11f_constant_utf8_info_t *utf8_info =
            (r11f_constant_utf8_info_t*)cpinfo;
        if (utf8_info->length == 4 &&
            !strncmp((char*)utf8_info->bytes, "Code", 4)) {
            preprocess_code_attribute(attribute_info);
        }
    }
#endif

    return R11F_success;
}

#ifdef R11F_LITTLE_ENDIAN
static void preprocess_code_attribute(r11f_attribute_info_t *attribute) {
    uint8_t *info = attribute->info;
    flip2_unaligned(info); /* Code_attribute->max_stack */
    flip2_unaligned(info + 2); /* Code_attribute->max_locals */
    flip4_unaligned(info + 4); /* Code_attribute->code_length */
    uint32_t code_length = read_unaligned4(info + 4);

    /* info = Code_attribute->exception_table_length */
    info = info + 8 + code_length;
    flip2_unaligned(info); /* Code_attribute->exception_table_length */
    uint16_t exception_table_length = read_unaligned2(info);

    /* info = Code_attribute->exception_table */
    info = info + 2;
    for (uint16_t i = 0; i < exception_table_length; i++) {
        flip2_unaligned(info); /* exception_table->start_pc */
        flip2_unaligned(info + 2); /* exception_table->end_pc */
        flip2_unaligned(info + 4); /* exception_table->handler_pc */
        flip2_unaligned(info + 6); /* exception_table->catch_type */
        info = info + 8;
    }

    /* TODO: we ignore Code_attribute->attributes for now */
}
#endif
