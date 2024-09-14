#ifndef R11F_INTERNAL_ALLOC_H
#define R11F_INTERNAL_ALLOC_H

#include <stddef.h>
#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

R11F_EXPORT void* r11f_alloc(size_t size);
R11F_EXPORT void* r11f_alloc_zeroed(size_t size);
R11F_EXPORT void r11f_free(void *ptr);

typedef struct {
    size_t heap_mem_used;
    size_t alloc_count;
    size_t dealloc_count;
    size_t fail_count;
} r11f_memstat_t;

R11F_EXPORT r11f_memstat_t r11f_memstat_get(void);
R11F_EXPORT void r11f_memstat_clear(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_INTERNAL_ALLOC_H */
