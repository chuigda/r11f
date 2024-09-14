#include "cfdump.h"

#include <inttypes.h>
#include <stdio.h>
#include "bytecode.h"
#include "class.h"
#include "class/attrib.h"
#include "class/cpool.h"
#include "defs.h"

static void
dump_access_flags(FILE *fp, char const* prefix, uint16_t access_flags);
static void dump_class_info(FILE *fp,
                            char const* prefix,
                            r11f_class_t *clazz,
                            uint16_t index);
static void dump_method_info(FILE *fp,
                             r11f_class_t *clazz,
                             r11f_method_info_t *method_info);
static void dump_attribute_name(FILE *fp,
                                r11f_class_t *clazz,
                                r11f_attribute_info_t *attribute_info);
static void dump_attribute_info(FILE *fp,
                                char const *prefix,
                                r11f_attribute_info_t *attribute_info);
static void dump_name(FILE *fp,
                      r11f_class_t *clazz,
                      uint16_t index,
                      uint16_t parent_index);

R11F_EXPORT void
r11f_class_dump(FILE *fp, char const* filename, r11f_class_t *clazz) {
    fprintf(fp, "cf = %s\n", filename);
    fprintf(fp, "  cf->magic: 0x%08X\n", clazz->magic);
    fprintf(fp, "  cf->major_version: %d\n", clazz->major_version);
    fprintf(fp, "  cf->minor_version: %d\n", clazz->minor_version);
    fprintf(
        fp,
        "  cf->constant_pool_count: %d\n",
        clazz->constant_pool_count
    );

    dump_access_flags(fp, "  cf->access_flags: ", clazz->access_flags);

    dump_class_info(fp, "  cf->this_class: ", clazz, clazz->this_class);
    dump_class_info(fp, "  cf->super_class: ", clazz, clazz->super_class);

    fprintf(
        fp,
        "  cf->interfaces_count: %d\n",
        clazz->interfaces_count
    );

    fprintf(fp, "  cf->fields_count: %d\n", clazz->fields_count);
    fprintf(fp, "  cf->methods_count: %d\n", clazz->methods_count);
    for (uint16_t i = 0; i < clazz->methods_count; i++) {
        dump_method_info(fp, clazz, clazz->methods[i]);
    }

    fprintf(
        fp,
        "  cf->attributes_count: %d\n",
        clazz->attributes_count
    );
    for (uint16_t i = 0; i < clazz->attributes_count; i++) {
        r11f_attribute_info_t *attribute_info = clazz->attributes[i];
        fprintf(fp, "    [%d] ", i);
        dump_attribute_name(fp, clazz, attribute_info);
        dump_attribute_info(fp, "        ", attribute_info);
    }
}

R11F_EXPORT void r11f_dump_constant_pool_item(FILE *fp, void *cpinfo) {
    if (!cpinfo) {
        fprintf(fp, "NULL");
        return;
    }

    uint8_t tag = ((r11f_cpinfo_t*)cpinfo)->tag;
    switch (tag) {
        case R11F_CONSTANT_Class: {
            r11f_constant_class_info_t *class_info = cpinfo;
            fprintf(
                fp,
                "Class name_index=#%d",
                class_info->name_index
            );
            break;
        }
        case R11F_CONSTANT_Fieldref: {
            r11f_constant_fieldref_info_t *fieldref_info = cpinfo;
            fprintf(
                fp,
                "Fieldref class_index=#%d name_and_type_index=#%d",
                fieldref_info->class_index,
                fieldref_info->name_and_type_index
            );
            break;
        }
        case R11F_CONSTANT_Methodref: {
            r11f_constant_methodref_info_t *methodref_info = cpinfo;
            fprintf(
                fp,
                "Methodref class_index=#%d name_and_type_index=#%d",
                methodref_info->class_index,
                methodref_info->name_and_type_index
            );
            break;
        }
        case R11F_CONSTANT_InterfaceMethodref: {
            r11f_constant_interface_methodref_info_t *interface_methodref_info = cpinfo;
            fprintf(
                fp,
                "InterfaceMethodref class_index=#%d name_and_type_index=#%d",
                interface_methodref_info->class_index,
                interface_methodref_info->name_and_type_index
            );
            break;
        }
        case R11F_CONSTANT_String: {
            r11f_constant_string_info_t *string_info = cpinfo;
            fprintf(
                fp,
                "String string_index=#%d",
                string_info->string_index
            );
            break;
        }
        case R11F_CONSTANT_Integer: {
            r11f_constant_integer_info_t *integer_info = cpinfo;
            fprintf(
                fp,
                "Integer bytes=0x%08X",
                integer_info->bytes
            );
            break;
        }
        case R11F_CONSTANT_Float: {
            r11f_constant_float_info_t *float_info = cpinfo;
            fprintf(
                fp,
                "Float bytes=0x%08X",
                float_info->bytes
            );
            break;
        }
        case R11F_CONSTANT_Long: {
            r11f_constant_long_info_t *long_info = cpinfo;
            uint64_t value =
                ((uint64_t)long_info->high_bytes << 32) | long_info->low_bytes;
            fprintf(
                fp,
                "Long high_bytes=0x%08X low_bytes=0x%08X value=%" PRIu64,
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
                fp,
                "Double high_bytes=0x%08X low_bytes=0x%08X value=%f",
                double_info->high_bytes,
                double_info->low_bytes,
                dvalue
            );
            break;
        }
        case R11F_CONSTANT_NameAndType: {
            r11f_constant_name_and_type_info_t *name_and_type_info = cpinfo;
            fprintf(
                fp,
                "NameAndType name_index=#%d descriptor_index=#%d",
                name_and_type_info->name_index,
                name_and_type_info->descriptor_index
            );
            break;
        }
        case R11F_CONSTANT_Utf8: {
            r11f_constant_utf8_info_t *utf8_info = cpinfo;
            fprintf(
                fp,
                "Utf8 length=%d bytes=\"%.*s\"",
                utf8_info->length,
                utf8_info->length,
                utf8_info->bytes
            );
            break;
        }
        default: {
            fprintf(fp, "INVALID (tag %d)", tag);
            break;
        }
    }
}

static void
dump_access_flags(FILE *fp, char const* prefix, uint16_t access_flags) {
    fprintf(fp, "%s", prefix);
    if (access_flags & R11F_ACC_PUBLIC) {
        fprintf(fp, "ACC_PUBLIC ");
    }
    if (access_flags & R11F_ACC_PRIVATE) {
        fprintf(fp, "ACC_PRIVATE ");
    }
    if (access_flags & R11F_ACC_PROTECTED) {
        fprintf(fp, "ACC_PROTECTED ");
    }
    if (access_flags & R11F_ACC_STATIC) {
        fprintf(fp, "ACC_STATIC ");
    }
    if (access_flags & R11F_ACC_FINAL) {
        fprintf(fp, "ACC_FINAL ");
    }
    if (access_flags & R11F_ACC_SUPER) {
        fprintf(fp, "ACC_SUPER|ACC_SYNCHRONIZED ");
    }
    if (access_flags & R11F_ACC_BRIDGE) {
        fprintf(fp, "ACC_BRIDGE ");
    }
    if (access_flags & R11F_ACC_VARARGS) {
        fprintf(fp, "ACC_VARARGS ");
    }
    if (access_flags & R11F_ACC_NATIVE) {
        fprintf(fp, "ACC_NATIVE ");
    }
    if (access_flags & R11F_ACC_INTERFACE) {
        fprintf(fp, "ACC_INTERFACE ");
    }
    if (access_flags & R11F_ACC_ABSTRACT) {
        fprintf(fp, "ACC_ABSTRACT ");
    }
    if (access_flags & R11F_ACC_SYNTHETIC) {
        fprintf(fp, "ACC_SYNTHETIC ");
    }
    if (access_flags & R11F_ACC_ANNOTATION) {
        fprintf(fp, "ACC_ANNOTATION ");
    }
    if (access_flags & R11F_ACC_ENUM) {
        fprintf(fp, "ACC_ENUM ");
    }
    fprintf(fp, "\n");
}

static void dump_class_info(FILE *fp,
                            char const* prefix,
                            r11f_class_t *clazz,
                            uint16_t index) {
    fprintf(fp, "%s", prefix);
    if (index == 0) {
        fprintf(fp, "#0 NULL\n");
        return;
    }

    if (index >= clazz->constant_pool_count) {
        fprintf(fp, "out of bounds (%d)\n", index);
        return;
    }

    r11f_constant_class_info_t *class_info =
        clazz->constant_pool[index];
    if (class_info->tag != R11F_CONSTANT_Class) {
        fprintf(
            fp,
            "invalid tag (%d), constant pool entry (%d)\n",
            class_info->tag,
            index
        );
        return;
    }

    dump_name(fp, clazz, class_info->name_index, index);
}

static void dump_method_info(FILE *fp,
                             r11f_class_t *clazz,
                             r11f_method_info_t *method_info) {
    fprintf(fp, "    method_info->name_index: ");
    dump_name(fp, clazz, method_info->name_index, 0);
    dump_access_flags(
        fp,
        "      method_info->access_flags: ",
        method_info->access_flags
    );

    fprintf(fp, "      method_info->descriptor_index: ");
    dump_name(fp, clazz, method_info->descriptor_index, 0);

    fprintf(
        fp,
        "      method_info->attributes_count: %d\n",
        method_info->attributes_count
    );

    for (uint16_t i = 0; i < method_info->attributes_count; i++) {
        r11f_attribute_info_t *attribute_info = method_info->attributes[i];
        fprintf(fp, "        [%d] ", i);
        dump_attribute_name(fp, clazz, attribute_info);
        dump_attribute_info(fp, "            ", attribute_info);
    }

    fprintf(fp, "      method_info->attribute_code:\n");
    r11f_disassemble(clazz, method_info, 8);
}

static void dump_attribute_name(FILE *fp,
                                r11f_class_t *clazz,
                                r11f_attribute_info_t *attribute_info) {
    fprintf(fp, "attribute_info->attribute_name_index: ");
    dump_name(fp, clazz, attribute_info->attribute_name_index, 0);
}

static void dump_attribute_info(FILE *fp,
                                char const *prefix,
                                r11f_attribute_info_t *attribute_info) {
    fprintf(fp, "%s", prefix);
    fprintf(
        fp,
        "attribute_info->attribute_length: %d\n",
        attribute_info->attribute_length
    );

    if (attribute_info->attribute_length > 0) {
        fprintf(fp, "%s", prefix);
        fprintf(fp, "attribute_info->info: ");
        for (uint32_t i = 0; i < attribute_info->attribute_length; i++) {
            fprintf(fp, "%02X ", attribute_info->info[i]);
        }
        fprintf(fp, "\n");
    }
}

void dump_name(FILE *fp,
               r11f_class_t *clazz,
               uint16_t index,
               uint16_t parent_index) {
    if (index >= clazz->constant_pool_count) {
        fprintf(fp, "name_index out of bounds (%d)\n", index);
        return;
    }

    r11f_constant_utf8_info_t *utf8_info = clazz->constant_pool[index];
    if (utf8_info->tag != R11F_CONSTANT_Utf8) {
        fprintf(
            fp,
            "name invalid tag (%d), constant pool entry (%d)\n",
            utf8_info->tag,
            index
        );
        return;
    }

    if (parent_index) {
        fprintf(
            fp,
            "#%d -> #%d %.*s\n",
            index,
            parent_index,
            utf8_info->length,
            utf8_info->bytes
        );
    }
    else {
        fprintf(
            fp,
            "#%d %.*s\n",
            index,
            utf8_info->length,
            utf8_info->bytes
        );
    }
}
