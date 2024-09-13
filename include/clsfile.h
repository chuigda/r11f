#ifndef R11F_CLASSFILE_H
#define R11F_CLASSFILE_H

#include <stdio.h>

#include "defs.h"
#include "error.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

R11F_EXPORT r11f_error_t
r11f_classfile_read(FILE *file, r11f_class_t *clazz);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSFILE_H */
