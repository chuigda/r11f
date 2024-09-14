#ifndef R11F_CLASSDUMP_H
#define R11F_CLASSDUMP_H

#include <stdio.h>

#include "defs.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

R11F_EXPORT void
r11f_class_dump(FILE *fp, char const* filename, r11f_class_t *classfile);

R11F_EXPORT void
r11f_dump_constant_pool_item(FILE *fp, void *cpinfo);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSDUMP_H */
