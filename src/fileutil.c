#include "fileutil.h"

#ifdef R11F_LITTLE_ENDIAN
static void flip2(uint16_t *value);
static void flip4(uint32_t *value);
#endif

R11F_INTERNAL bool read_u2(FILE *file, uint16_t *value) {
    int ret = fread(value, sizeof(uint16_t), 1, file);
    if (ret != 1) {
        return false;
    }

#ifdef R11F_LITTLE_ENDIAN
    flip2(value);
#endif

    return true;
}

R11F_INTERNAL bool read_u4(FILE *file, uint32_t *value) {
    int ret = fread(value, sizeof(uint32_t), 1, file);
    if (ret != 1) {
        return false;
    }

#ifdef R11F_LITTLE_ENDIAN
    flip4(value);
#endif

    return true;
}

#ifdef R11F_LITTLE_ENDIAN
static void flip2(uint16_t *value) {
    uint8_t *buf = (uint8_t *)value;
    uint8_t tmp = buf[0];
    buf[0] = buf[1];
    buf[1] = tmp;
}

static void flip4(uint32_t *value) {
    uint8_t *buf = (uint8_t *)value;
    uint8_t tmp = buf[0];
    buf[0] = buf[3];
    buf[3] = tmp;
    tmp = buf[1];
    buf[1] = buf[2];
    buf[2] = tmp;
}
#endif
