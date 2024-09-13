#ifndef R11F_ERROR_H
#define R11F_ERROR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t r11f_error_t;

enum {
    R11F_ERR_none = 0,
    R11F_ERR_out_of_memory = 1,
    R11F_ERR_malformed_classfile = 2
};

char const* r11f_explain_error(r11f_error_t error);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_ERROR_H */
