#include "class.h"

#include <stdlib.h>
#include <string.h>
#include "alloc.h"
#include "class/attrib.h"
#include "class/cpool.h"

R11F_EXPORT void r11f_class_cleanup(r11f_class_t *clazz) {
    if (clazz->constant_pool) {
        for (uint16_t i = 1; i < clazz->constant_pool_count; i++) {
            if (clazz->constant_pool[i]) {
                r11f_free(clazz->constant_pool[i]);
            }
        }
        r11f_free(clazz->constant_pool);
    }

    if (clazz->interfaces) {
        r11f_free(clazz->interfaces);
    }

    if (clazz->fields) {
        for (uint16_t i = 0; i < clazz->fields_count; i++) {
            r11f_field_info_t *field_info = clazz->fields[i];
            if (field_info->attributes) {
                for (uint16_t j = 0; j < field_info->attributes_count; j++) {
                    r11f_free(field_info->attributes[j]);
                }
                r11f_free(field_info->attributes);
            }
            r11f_free(field_info);
        }
        r11f_free(clazz->fields);
    }

    if (clazz->methods) {
        for (uint16_t i = 0; i < clazz->methods_count; i++) {
            r11f_method_info_t *method_info = clazz->methods[i];
            if (method_info->attributes) {
                for (uint16_t j = 0; j < method_info->attributes_count; j++) {
                    r11f_free(method_info->attributes[j]);
                }
                r11f_free(method_info->attributes);
            }
            r11f_free(method_info);
        }
        r11f_free(clazz->methods);
    }

    if (clazz->attributes) {
        for (uint16_t i = 0; i < clazz->attributes_count; i++) {
            r11f_free(clazz->attributes[i]);
        }
        r11f_free(clazz->attributes);
    }
}

R11F_EXPORT r11f_method_info_t*
r11f_class_resolve_method(r11f_class_t *clazz,
                          char const *name,
                          uint16_t name_len,
                          char const *descriptor,
                          uint16_t descriptor_len) {
    for (uint16_t i = 0; i < clazz->methods_count; i++) {
        r11f_method_info_t *method_info = clazz->methods[i];
        r11f_constant_utf8_info_t *name_info =
            clazz->constant_pool[method_info->name_index];
        r11f_constant_utf8_info_t *desc_info =
            clazz->constant_pool[method_info->descriptor_index];

        if (name_info->length == name_len &&
            !strncmp((char*)name_info->bytes, name, name_len) &&
            desc_info->length == descriptor_len &&
            !strncmp((char*)desc_info->bytes, descriptor, descriptor_len)) {
            return method_info;
        }
    }

    return NULL;
}

R11F_EXPORT r11f_attribute_info_t*
r11f_method_find_attribute(r11f_class_t *clazz,
                           r11f_method_info_t *method_info,
                           char const *name) {
    for (uint16_t i = 0; i < method_info->attributes_count; i++) {
        r11f_attribute_info_t *attr_info = method_info->attributes[i];
        r11f_constant_utf8_info_t *attr_name_info =
            clazz->constant_pool[attr_info->attribute_name_index];

        if (attr_name_info->length == strlen(name) &&
            !strncmp((char*)attr_name_info->bytes, name, strlen(name))) {
            return attr_info;
        }
    }

    return NULL;
}
