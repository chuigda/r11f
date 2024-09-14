#ifndef R11F_MEMSTAT_H
#define R11F_MEMSTAT_H

#include "defs.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#endif /* R11F_MEMSTAT_H */
