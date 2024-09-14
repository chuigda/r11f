#ifndef R11F_INTERNAL_ALLOC_H
#define R11F_INTERNAL_ALLOC_H

#include <stddef.h>
#include "defs.h"

R11F_INTERNAL void* r11f_alloc(size_t size);
R11F_INTERNAL void* r11f_alloc_zeroed(size_t size);
R11F_INTERNAL void r11f_free(void *ptr);

#endif /* R11F_INTERNAL_ALLOC_H */
