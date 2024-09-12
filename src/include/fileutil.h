#ifndef R11F_INTERNAL_FILEUTIL_H
#define R11F_INTERNAL_FILEUTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "internal.h"

R11F_INTERNAL bool read_u2(FILE *file, uint16_t *value);
R11F_INTERNAL bool read_u4(FILE *file, uint32_t *value);

#endif /* R11F_INTERNAL_FILEUTIL_H */
