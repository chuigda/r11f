#ifndef R11F_CLASSDUMP_H
#define R11F_CLASSDUMP_H

#include "defs.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

R11F_EXPORT void
r11f_classfile_dump(char const* filename, r11f_classfile_t *classfile);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASSDUMP_H */
