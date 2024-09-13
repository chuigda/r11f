#ifndef R11F_INTERNAL_BYTEUTIL_H
#define R11F_INTERNAL_BYTEUTIL_H

#include "defs.h"
#include <stdint.h>

R11F_INTERNAL void flip2(uint16_t *value);
R11F_INTERNAL void flip4(uint32_t *value);

R11F_INTERNAL uint16_t read_unaligned2(void* addr);
R11F_INTERNAL void write_unaligned2(void* addr, uint16_t value);
R11F_INTERNAL uint32_t read_unaligned4(void* addr);
R11F_INTERNAL void write_unaligned4(void* addr, uint32_t value);

R11F_INTERNAL void flip2_unaligned(void* addr);
R11F_INTERNAL void flip4_unaligned(void* addr);

#endif /* R11F_INTERNAL_BYTEUTIL_H */
