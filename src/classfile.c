#include "classfile.h"

#include <inttypes.h>
#include <stdlib.h>
#include "fileutil.h"

#define CHKRET(expr) if (!(expr)) { return false; }

static bool read_header(FILE *file, r11f_classfile_t *classfile);
static bool read_constant_pool(FILE *file, r11f_classfile_t *classfile);
static bool read_classinfo(FILE *file, r11f_classfile_t *classfile);
static bool read_interfaces(FILE *file, r11f_classfile_t *classfile);
static bool read_fields(FILE *file, r11f_classfile_t *classfile);
static bool read_methods(FILE *file, r11f_classfile_t *classfile);
static bool read_attributes(FILE *file, r11f_classfile_t *classfile);
static bool
imp_read_attributes(FILE *file, size_t n, r11f_attribute_info_t **attributes);
static void dump_access_flags(char const* prefix, uint16_t access_flags);
static void dump_constant_pool_item(void *cpinfo);
static void dump_class_info(char const* prefix,
                            r11f_classfile_t *classfile,
                            uint16_t index);

bool r11f_classfile_read(FILE *file, r11f_classfile_t *classfile) {
    CHKRET(read_header(file, classfile))
    CHKRET(read_constant_pool(file, classfile))
    CHKRET(read_classinfo(file, classfile))
    CHKRET(read_interfaces(file, classfile))
    CHKRET(read_fields(file, classfile))
    CHKRET(read_methods(file, classfile))
    CHKRET(read_attributes(file, classfile))

    return true;
}

void r11f_classfile_cleanup(r11f_classfile_t *classfile) {
    if (classfile->constant_pool) {
        for (uint16_t i = 1; i < classfile->constant_pool_count; i++) {
            if (classfile->constant_pool[i]) {
                free(classfile->constant_pool[i]);
            }
        }
        free(classfile->constant_pool);
    }

    if (classfile->interfaces) {
        free(classfile->interfaces);
    }

    if (classfile->fields) {
        for (uint16_t i = 0; i < classfile->fields_count; i++) {
            r11f_field_info_t *field_info = classfile->fields[i];
            if (field_info->attributes) {
                for (uint16_t j = 0; j < field_info->attributes_count; j++) {
                    free(field_info->attributes[j]);
                }
                free(field_info->attributes);
            }
            free(field_info);
        }
        free(classfile->fields);
    }

    if (classfile->methods) {
        for (uint16_t i = 0; i < classfile->methods_count; i++) {
            r11f_method_info_t *method_info = classfile->methods[i];
            if (method_info->attributes) {
                for (uint16_t j = 0; j < method_info->attributes_count; j++) {
                    free(method_info->attributes[j]);
                }
                free(method_info->attributes);
            }
            free(method_info);
        }
        free(classfile->methods);
    }

    if (classfile->attributes) {
        for (uint16_t i = 0; i < classfile->attributes_count; i++) {
            free(classfile->attributes[i]);
        }
        free(classfile->attributes);
    }
}

void r11f_classfile_dump(char const* filename, r11f_classfile_t *classfile) {
    fprintf(stderr, "cf = %s\n", filename);
    fprintf(stderr, "  cf->magic: 0x%08X\n", classfile->magic);
    fprintf(stderr, "  cf->major_version: %d\n", classfile->major_version);
    fprintf(stderr, "  cf->minor_version: %d\n", classfile->minor_version);
    fprintf(
        stderr,
        "  cf->constant_pool_count: %d\n",
        classfile->constant_pool_count
    );
    for (uint16_t i = 1; i < classfile->constant_pool_count; i++) {
        fprintf(stderr, "    [%d] ", i);
        dump_constant_pool_item(classfile->constant_pool[i]);
    }

    dump_access_flags("  cf->access_flags: ", classfile->access_flags);

    dump_class_info("  cf->this_class: ", classfile, classfile->this_class);
    dump_class_info("  cf->super_class: ", classfile, classfile->super_class);

    fprintf(
        stderr,
        "  cf->interfaces_count: %d\n",
        classfile->interfaces_count
    );
    fprintf(stderr, "  cf->fields_count: %d\n", classfile->fields_count);
    fprintf(stderr, "  cf->methods_count: %d\n", classfile->methods_count);
    fprintf(
        stderr,
        "  cf->attributes_count: %d\n",
        classfile->attributes_count
    );
}

static bool read_header(FILE *file, r11f_classfile_t *classfile) {
    if (!read_u4(file, &classfile->magic)
        || classfile->magic != 0xCAFEBABE) {
        return false;
    }

    if (!read_u2(file, &classfile->minor_version)
        || classfile->minor_version != 0) {
        return false;
    }

    if (!read_u2(file, &classfile->major_version)
        || classfile->major_version != 52) {
        return false;
    }

    return true;
}

static bool read_constant_pool(FILE *file, r11f_classfile_t *classfile) {
    CHKRET(read_u2(file, &classfile->constant_pool_count))
    CHKRET(classfile->constant_pool = calloc(
        classfile->constant_pool_count,
        sizeof(r11f_cpinfo_t *)
    ))

    for (uint16_t i = 1; i < classfile->constant_pool_count; i++) {
        uint8_t tag;
        CHKRET(read_byte(file, &tag))

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
                fprintf(
                    stderr,
                    "classfile_read: unknown constant pool tag: %d\n",
                    tag
                );
                return false;
        }

        if (size) {
            CHKRET(classfile->constant_pool[i] = malloc(size))
            ((r11f_cpinfo_t*)classfile->constant_pool[i])->tag = tag;
        }

        switch (tag) {
            case R11F_CONSTANT_Class: {
                r11f_constant_class_info_t *class_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(file, &class_info->name_index))
                break;
            }
            case R11F_CONSTANT_Fieldref: {
                r11f_constant_fieldref_info_t *fieldref_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(file, &fieldref_info->class_index))
                CHKRET(read_u2(file, &fieldref_info->name_and_type_index))
                break;
            }
            case R11F_CONSTANT_Methodref: {
                r11f_constant_methodref_info_t *methodref_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(file, &methodref_info->class_index))
                CHKRET(read_u2(file, &methodref_info->name_and_type_index))
                break;
            }
            case R11F_CONSTANT_InterfaceMethodref: {
                r11f_constant_interface_methodref_info_t *interface_methodref_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(
                    file,
                    &interface_methodref_info->class_index
                ))
                CHKRET(read_u2(
                    file,
                    &interface_methodref_info->name_and_type_index
                ))
                break;
            }
            case R11F_CONSTANT_String: {
                r11f_constant_string_info_t *string_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(file, &string_info->string_index))
                break;
            }
            case R11F_CONSTANT_Integer: {
                r11f_constant_integer_info_t *integer_info =
                    classfile->constant_pool[i];
                CHKRET(read_u4(file, &integer_info->bytes))
                break;
            }
            case R11F_CONSTANT_Float: {
                r11f_constant_float_info_t *float_info =
                    classfile->constant_pool[i];
                CHKRET(read_u4(file, &float_info->bytes))
                break;
            }
            case R11F_CONSTANT_Long: {
                r11f_constant_long_info_t *long_info =
                    classfile->constant_pool[i];
                CHKRET(read_u4(file, &long_info->high_bytes))
                CHKRET(read_u4(file, &long_info->low_bytes))
                i++;
                break;
            }
            case R11F_CONSTANT_Double: {
                r11f_constant_double_info_t *double_info =
                    classfile->constant_pool[i];
                CHKRET(read_u4(file, &double_info->high_bytes))
                CHKRET(read_u4(file, &double_info->low_bytes))
                i++;
                break;
            }
            case R11F_CONSTANT_NameAndType: {
                r11f_constant_name_and_type_info_t *name_and_type_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(file, &name_and_type_info->name_index))
                CHKRET(read_u2(file, &name_and_type_info->descriptor_index))
                break;
            }
            case R11F_CONSTANT_Utf8: {
                uint16_t length;
                CHKRET(read_u2(file, &length))

                r11f_constant_utf8_info_t *utf8_info = malloc(
                    sizeof(r11f_constant_utf8_info_t) + length
                );
                CHKRET(classfile->constant_pool[i] = utf8_info)

                utf8_info->tag = R11F_CONSTANT_Utf8;
                utf8_info->length = length;
                CHKRET(read_byte_array(file, utf8_info->bytes, length))
                break;
            }
            case R11F_CONSTANT_MethodHandle: {
                r11f_constant_method_handle_info_t *method_handle_info =
                    classfile->constant_pool[i];
                CHKRET(read_byte(file, &method_handle_info->reference_kind))
                CHKRET(read_u2(file, &method_handle_info->reference_index))
                break;
            }
            case R11F_CONSTANT_MethodType: {
                r11f_constant_method_type_info_t *method_type_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(file, &method_type_info->descriptor_index))
                break;
            }
            case R11F_CONSTANT_InvokeDynamic: {
                r11f_constant_invoke_dynamic_info_t *invoke_dynamic_info =
                    classfile->constant_pool[i];
                CHKRET(read_u2(
                    file,
                    &invoke_dynamic_info->bootstrap_method_attr_index
                ))
                CHKRET(read_u2(
                    file,
                    &invoke_dynamic_info->name_and_type_index
                ))
                break;
            }
        }
    }

    return true;
}

static bool read_classinfo(FILE *file, r11f_classfile_t *classfile) {
    CHKRET(read_u2(file, &classfile->access_flags))
    CHKRET(read_u2(file, &classfile->this_class))
    CHKRET(read_u2(file, &classfile->super_class))

    return true;
}

static bool read_interfaces(FILE *file, r11f_classfile_t *classfile) {
    CHKRET(read_u2(file, &classfile->interfaces_count))
    CHKRET(classfile->interfaces = malloc(
        classfile->interfaces_count * sizeof(uint16_t)
    ))

    for (uint16_t i = 0; i < classfile->interfaces_count; i++) {
        CHKRET(read_u2(file, &classfile->interfaces[i]))
    }

    return true;
}

static bool read_fields(FILE *file, r11f_classfile_t *classfile) {
    CHKRET(read_u2(file, &classfile->fields_count))
    CHKRET(classfile->fields = malloc(
        classfile->fields_count * sizeof(r11f_field_info_t *)
    ))

    for (uint16_t i = 0; i < classfile->fields_count; i++) {
        r11f_field_info_t *field_info = malloc(sizeof(r11f_field_info_t));
        CHKRET(classfile->fields[i] = field_info)

        CHKRET(read_u2(file, &field_info->access_flags))
        CHKRET(read_u2(file, &field_info->name_index))
        CHKRET(read_u2(file, &field_info->descriptor_index))
        CHKRET(read_u2(file, &field_info->attributes_count))
        CHKRET(field_info->attributes = malloc(
            field_info->attributes_count * sizeof(r11f_attribute_info_t*)
        ))

        CHKRET(imp_read_attributes(
            file,
            field_info->attributes_count,
            field_info->attributes
        ))
    }

    return true;
}

static bool read_methods(FILE *file, r11f_classfile_t *classfile) {
    CHKRET(read_u2(file, &classfile->methods_count))
    CHKRET(classfile->methods = malloc(
        classfile->methods_count * sizeof(r11f_method_info_t *)
    ))

    for (uint16_t i = 0; i < classfile->methods_count; i++) {
        r11f_method_info_t *method_info = malloc(sizeof(r11f_method_info_t));
        CHKRET(classfile->methods[i] = method_info)

        CHKRET(read_u2(file, &method_info->access_flags))
        CHKRET(read_u2(file, &method_info->name_index))
        CHKRET(read_u2(file, &method_info->descriptor_index))
        CHKRET(read_u2(file, &method_info->attributes_count))
        CHKRET(method_info->attributes = malloc(
            method_info->attributes_count * sizeof(r11f_attribute_info_t*)
        ))

        CHKRET(imp_read_attributes(
            file,
            method_info->attributes_count,
            method_info->attributes
        ))
    }

    return true;
}

static bool read_attributes(FILE *file, r11f_classfile_t *classfile) {
    CHKRET(read_u2(file, &classfile->attributes_count))
    CHKRET(classfile->attributes = malloc(
        classfile->attributes_count * sizeof(r11f_attribute_info_t*)
    ))

    CHKRET(imp_read_attributes(
        file,
        classfile->attributes_count,
        classfile->attributes
    ))
    return true;
}

static bool
imp_read_attributes(FILE *file, size_t n, r11f_attribute_info_t **attributes) {
    for (size_t i = 0; i < n; i++) {
        uint16_t attribute_name_index;
        CHKRET(read_u2(file, &attribute_name_index))

        uint32_t attribute_length;
        CHKRET(read_u4(file, &attribute_length))

        r11f_attribute_info_t *attribute_info = malloc(
            sizeof(r11f_attribute_info_t) + attribute_length
        );
        CHKRET(attributes[i] = attribute_info)

        attribute_info->attribute_name_index = attribute_name_index;
        attribute_info->attribute_length = attribute_length;
        CHKRET(read_byte_array(file, attribute_info->info, attribute_length))
    }

    return true;
}

static void dump_access_flags(char const* prefix, uint16_t access_flags) {
    fprintf(stderr, "%s", prefix);
    if (access_flags & R11F_ACC_PUBLIC) {
        fprintf(stderr, "ACC_PUBLIC ");
    }
    if (access_flags & R11F_ACC_FINAL) {
        fprintf(stderr, "ACC_FINAL ");
    }
    if (access_flags & R11F_ACC_SUPER) {
        fprintf(stderr, "ACC_SUPER ");
    }
    if (access_flags & R11F_ACC_INTERFACE) {
        fprintf(stderr, "ACC_INTERFACE ");
    }
    if (access_flags & R11F_ACC_ABSTRACT) {
        fprintf(stderr, "ACC_ABSTRACT ");
    }
    if (access_flags & R11F_ACC_SYNTHETIC) {
        fprintf(stderr, "ACC_SYNTHETIC ");
    }
    if (access_flags & R11F_ACC_ANNOTATION) {
        fprintf(stderr, "ACC_ANNOTATION ");
    }
    if (access_flags & R11F_ACC_ENUM) {
        fprintf(stderr, "ACC_ENUM ");
    }
    fprintf(stderr, "\n");
}

static void dump_constant_pool_item(void *cpinfo) {
    if (!cpinfo) {
        fprintf(stderr, "NULL\n");
        return;
    }

    uint8_t tag = ((r11f_cpinfo_t*)cpinfo)->tag;
    switch (tag) {
        case R11F_CONSTANT_Class: {
            r11f_constant_class_info_t *class_info = cpinfo;
            fprintf(
                stderr,
                "Class name_index=#%d\n",
                class_info->name_index
            );
            break;
        }
        case R11F_CONSTANT_Fieldref: {
            r11f_constant_fieldref_info_t *fieldref_info = cpinfo;
            fprintf(
                stderr,
                "Fieldref class_index=#%d name_and_type_index=#%d\n",
                fieldref_info->class_index,
                fieldref_info->name_and_type_index
            );
            break;
        }
        case R11F_CONSTANT_Methodref: {
            r11f_constant_methodref_info_t *methodref_info = cpinfo;
            fprintf(
                stderr,
                "Methodref class_index=#%d name_and_type_index=#%d\n",
                methodref_info->class_index,
                methodref_info->name_and_type_index
            );
            break;
        }
        case R11F_CONSTANT_InterfaceMethodref: {
            r11f_constant_interface_methodref_info_t *interface_methodref_info = cpinfo;
            fprintf(
                stderr,
                "InterfaceMethodref class_index=#%d name_and_type_index=#%d\n",
                interface_methodref_info->class_index,
                interface_methodref_info->name_and_type_index
            );
            break;
        }
        case R11F_CONSTANT_String: {
            r11f_constant_string_info_t *string_info = cpinfo;
            fprintf(
                stderr,
                "String string_index=#%d\n",
                string_info->string_index
            );
            break;
        }
        case R11F_CONSTANT_Integer: {
            r11f_constant_integer_info_t *integer_info = cpinfo;
            fprintf(
                stderr,
                "Integer bytes=0x%08X\n",
                integer_info->bytes
            );
            break;
        }
        case R11F_CONSTANT_Float: {
            r11f_constant_float_info_t *float_info = cpinfo;
            fprintf(
                stderr,
                "Float bytes=0x%08X\n",
                float_info->bytes
            );
            break;
        }
        case R11F_CONSTANT_Long: {
            r11f_constant_long_info_t *long_info = cpinfo;
            uint64_t value =
                ((uint64_t)long_info->high_bytes << 32) | long_info->low_bytes;
            fprintf(
                stderr,
                "Long high_bytes=0x%08X low_bytes=0x%08X value=%" PRIu64 "\n",
                long_info->high_bytes,
                long_info->low_bytes,
                value
            );
            break;
        }
        case R11F_CONSTANT_Double: {
            r11f_constant_double_info_t *double_info = cpinfo;
            uint64_t value =
                ((uint64_t)double_info->high_bytes << 32) | double_info->low_bytes;
            double dvalue = *(double*)&value;
            fprintf(
                stderr,
                "Double high_bytes=0x%08X low_bytes=0x%08X value=%f\n",
                double_info->high_bytes,
                double_info->low_bytes,
                dvalue
            );
            break;
        }
        case R11F_CONSTANT_NameAndType: {
            r11f_constant_name_and_type_info_t *name_and_type_info = cpinfo;
            fprintf(
                stderr,
                "NameAndType name_index=#%d descriptor_index=#%d\n",
                name_and_type_info->name_index,
                name_and_type_info->descriptor_index
            );
            break;
        }
        case R11F_CONSTANT_Utf8: {
            r11f_constant_utf8_info_t *utf8_info = cpinfo;
            fprintf(
                stderr,
                "Utf8 length=%d bytes=\"%.*s\"\n",
                utf8_info->length,
                utf8_info->length,
                utf8_info->bytes
            );
            break;
        }
        default: {
            fprintf(stderr, "INVALID (tag %d)\n", tag);
            break;
        }
    }
}

static void dump_class_info(char const* prefix,
                            r11f_classfile_t *classfile,
                            uint16_t index) {
    fprintf(stderr, "%s", prefix);
    if (index == 0) {
        fprintf(stderr, "#0 NULL\n");
        return;
    }

    if (index >= classfile->constant_pool_count) {
        fprintf(stderr, "out of bounds (%d)\n", index);
        return;
    }

    r11f_constant_class_info_t *class_info =
        classfile->constant_pool[index];
    if (class_info->tag != R11F_CONSTANT_Class) {
        fprintf(
            stderr,
            "invalid tag (%d), constant pool entry (%d)\n",
            class_info->tag,
            index
        );
        return;
    }

    if (class_info->name_index >= classfile->constant_pool_count) {
        fprintf(stderr, "name_index out of bounds (%d)\n", class_info->name_index);
        return;
    }

    r11f_constant_utf8_info_t *utf8_info =
        classfile->constant_pool[class_info->name_index];
    if (utf8_info->tag != R11F_CONSTANT_Utf8) {
        fprintf(
            stderr,
            "name invalid tag (%d), constant pool entry (%d)\n",
            utf8_info->tag,
            class_info->name_index
        );
        return;
    }

    fprintf(
        stderr,
        "#%d -> #%d %.*s\n",
        index,
        class_info->name_index,
        utf8_info->length,
        utf8_info->bytes
    );
}
