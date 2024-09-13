#ifndef R11F_CLASSDUMP_H
#define R11F_CLASSDUMP_H

#include "classfile.h"

#ifdef __cplusplus
extern "C" {
#endif

void r11f_classfile_dump(char const* filename, r11f_classfile_t *classfile);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSDUMP_H */
