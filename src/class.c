#include "class.h"

#include <stdlib.h>

R11F_EXPORT void r11f_class_cleanup(r11f_class_t *clazz) {
    if (clazz->constant_pool) {
        for (uint16_t i = 1; i < clazz->constant_pool_count; i++) {
            if (clazz->constant_pool[i]) {
                free(clazz->constant_pool[i]);
            }
        }
        free(clazz->constant_pool);
    }

    if (clazz->interfaces) {
        free(clazz->interfaces);
    }

    if (clazz->fields) {
        for (uint16_t i = 0; i < clazz->fields_count; i++) {
            r11f_field_info_t *field_info = clazz->fields[i];
            if (field_info->attributes) {
                for (uint16_t j = 0; j < field_info->attributes_count; j++) {
                    free(field_info->attributes[j]);
                }
                free(field_info->attributes);
            }
            free(field_info);
        }
        free(clazz->fields);
    }

    if (clazz->methods) {
        for (uint16_t i = 0; i < clazz->methods_count; i++) {
            r11f_method_info_t *method_info = clazz->methods[i];
            if (method_info->attributes) {
                for (uint16_t j = 0; j < method_info->attributes_count; j++) {
                    free(method_info->attributes[j]);
                }
                free(method_info->attributes);
            }
            free(method_info);
        }
        free(clazz->methods);
    }

    if (clazz->attributes) {
        for (uint16_t i = 0; i < clazz->attributes_count; i++) {
            free(clazz->attributes[i]);
        }
        free(clazz->attributes);
    }
}
