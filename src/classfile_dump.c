#include "classfile_dump.h"

#include <inttypes.h>
#include <stdio.h>

static void dump_access_flags(char const* prefix, uint16_t access_flags);
static void dump_constant_pool_item(void *cpinfo);
static void dump_class_info(char const* prefix,
                            r11f_classfile_t *classfile,
                            uint16_t index);
static void dump_method_info(r11f_classfile_t *classfile,
                             r11f_method_info_t *method_info);
static void dump_attribute_name(r11f_classfile_t *classfile,
                                r11f_attribute_info_t *attribute_info);
static void dump_attribute_info(char const *prefix,
                                r11f_attribute_info_t *attribute_info);
static void dump_name(r11f_classfile_t *classfile,
                      uint16_t index,
                      uint16_t parent_index);

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
    for (uint16_t i = 0; i < classfile->methods_count; i++) {
        dump_method_info(classfile, classfile->methods[i]);
    }

    fprintf(
        stderr,
        "  cf->attributes_count: %d\n",
        classfile->attributes_count
    );
    for (uint16_t i = 0; i < classfile->attributes_count; i++) {
        r11f_attribute_info_t *attribute_info = classfile->attributes[i];
        fprintf(stderr, "    [%d] ", i);
        dump_attribute_name(classfile, attribute_info);
        dump_attribute_info("        ", attribute_info);
    }
}

static void dump_access_flags(char const* prefix, uint16_t access_flags) {
    fprintf(stderr, "%s", prefix);
    if (access_flags & R11F_ACC_PUBLIC) {
        fprintf(stderr, "ACC_PUBLIC ");
    }
    if (access_flags & R11F_ACC_PRIVATE) {
        fprintf(stderr, "ACC_PRIVATE ");
    }
    if (access_flags & R11F_ACC_PROTECTED) {
        fprintf(stderr, "ACC_PROTECTED ");
    }
    if (access_flags & R11F_ACC_STATIC) {
        fprintf(stderr, "ACC_STATIC ");
    }
    if (access_flags & R11F_ACC_FINAL) {
        fprintf(stderr, "ACC_FINAL ");
    }
    if (access_flags & R11F_ACC_SUPER) {
        fprintf(stderr, "ACC_SUPER|ACC_SYNCHRONIZED ");
    }
    if (access_flags & R11F_ACC_BRIDGE) {
        fprintf(stderr, "ACC_BRIDGE ");
    }
    if (access_flags & R11F_ACC_VARARGS) {
        fprintf(stderr, "ACC_VARARGS ");
    }
    if (access_flags & R11F_ACC_NATIVE) {
        fprintf(stderr, "ACC_NATIVE ");
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

    dump_name(classfile, class_info->name_index, index);
}

static void dump_method_info(r11f_classfile_t *classfile,
                             r11f_method_info_t *method_info) {
    fprintf(stderr, "    method_info->name_index: ");
    dump_name(classfile, method_info->name_index, 0);
    dump_access_flags("      method_info->access_flags: ", method_info->access_flags);

    fprintf(stderr, "      method_info->descriptor_index: ");
    dump_name(classfile, method_info->descriptor_index, 0);

    fprintf(
        stderr,
        "      method_info->attributes_count: %d\n",
        method_info->attributes_count
    );
    for (uint16_t i = 0; i < method_info->attributes_count; i++) {
        r11f_attribute_info_t *attribute_info = method_info->attributes[i];
        fprintf(stderr, "        [%d] ", i);
        dump_attribute_name(classfile, attribute_info);
        dump_attribute_info("            ", attribute_info);
    }
}

static void dump_attribute_name(r11f_classfile_t *classfile,
                                r11f_attribute_info_t *attribute_info) {
    fprintf(stderr, "attribute_info->attribute_name_index: ");
    dump_name(classfile, attribute_info->attribute_name_index, 0);
}

static void dump_attribute_info(char const *prefix,
                                r11f_attribute_info_t *attribute_info) {
    fprintf(stderr, "%s", prefix);
    fprintf(
        stderr,
        "attribute_info->attribute_length: %d\n",
        attribute_info->attribute_length
    );

    if (attribute_info->attribute_length > 0) {
        fprintf(stderr, "%s", prefix);
        fprintf(stderr, "attribute_info->info: ");
        for (uint32_t i = 0; i < attribute_info->attribute_length; i++) {
            fprintf(stderr, "%02X ", attribute_info->info[i]);
        }
        fprintf(stderr, "\n");
    }
}

void dump_name(r11f_classfile_t *classfile,
               uint16_t index,
               uint16_t parent_index) {
    if (index >= classfile->constant_pool_count) {
        fprintf(stderr, "name_index out of bounds (%d)\n", index);
        return;
    }

    r11f_constant_utf8_info_t *utf8_info = classfile->constant_pool[index];
    if (utf8_info->tag != R11F_CONSTANT_Utf8) {
        fprintf(
            stderr,
            "name invalid tag (%d), constant pool entry (%d)\n",
            utf8_info->tag,
            index
        );
        return;
    }

    if (parent_index) {
        fprintf(
            stderr,
            "#%d -> #%d %.*s\n",
            index,
            parent_index,
            utf8_info->length,
            utf8_info->bytes
        );
    }
    else {
        fprintf(
            stderr,
            "#%d %.*s\n",
            index,
            utf8_info->length,
            utf8_info->bytes
        );
    }
}
