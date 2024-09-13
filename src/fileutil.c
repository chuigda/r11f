#include "fileutil.h"

#ifdef R11F_LITTLE_ENDIAN
#   include "byteutil.h"
#endif

R11F_INTERNAL bool read_byte(FILE *file, uint8_t *value) {
    size_t r = fread(value, sizeof(uint8_t), 1, file);
    return r == 1;
}

R11F_INTERNAL bool read_u2(FILE *file, uint16_t *value) {
    size_t r = fread(value, sizeof(uint16_t), 1, file);
    if (r != 1) {
        return false;
    }

#ifdef R11F_LITTLE_ENDIAN
    flip2(value);
#endif

    return true;
}

R11F_INTERNAL bool read_u4(FILE *file, uint32_t *value) {
    size_t r = fread(value, sizeof(uint32_t), 1, file);
    if (r != 1) {
        return false;
    }

#ifdef R11F_LITTLE_ENDIAN
    flip4(value);
#endif

    return true;
}

R11F_INTERNAL bool
read_byte_array(FILE *file, uint8_t *value, size_t length) {
    size_t r = fread(value, sizeof(uint8_t), length, file);
    return r == length;
}
