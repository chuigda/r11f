#ifndef R11F_CLASS_MANAGER_H
#define R11F_CLASS_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#include "defs.h"
#include "error.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

R11F_EXPORT r11f_classmgr_t *r11f_classmgr_alloc(void);
R11F_EXPORT r11f_classmgr_t *r11f_classmgr_alloc_hash_size(size_t hash_size);

R11F_EXPORT r11f_error_t r11f_classmgr_add_class(r11f_classmgr_t *mgr,
                                                 r11f_classfile_t *classfile,
                                                 uint32_t *classid);
R11F_EXPORT r11f_classfile_t *r11f_classmgr_find_class(r11f_classmgr_t *mgr,
                                                       char const *name);
R11F_EXPORT r11f_classfile_t *r11f_classmgr_find_class_id(r11f_classmgr_t *mgr,
                                                          uint32_t classid);
void r11f_classmgr_free(r11f_classmgr_t *mgr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_CLASS_MANAGER_H */
