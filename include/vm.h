#ifndef R11F_VM_H
#define R11F_VM_H

#include "defs.h"
#include "forward.h"
#include <error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char const* const* classpath;
    r11f_classmgr_t *classmgr;
    r11f_frame_t *current_frame;
} r11f_vm_t;

R11F_EXPORT
r11f_error_t r11f_vm_invoke_static(r11f_vm_t *vm,
                                   char const *class_name,
                                   char const *method_name,
                                   char const *method_descriptor,
                                   r11f_value_t *argv,
                                   void *output);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_VM_H */
