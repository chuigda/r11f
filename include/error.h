#ifndef R11F_ERROR_H
#define R11F_ERROR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t r11f_error_t;

enum {
    R11F_success = 0,
    R11F_ERR_out_of_memory = 1,
    R11F_ERR_malformed_classfile = 2,
    R11F_ERR_duplicate_class = 3,
};

char const* r11f_explain_error(r11f_error_t error);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_ERROR_H */
