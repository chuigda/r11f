#include "byteutil.h"

#include <string.h>
#include "defs.h"

R11F_INTERNAL void flip2(uint16_t *value) {
    uint8_t *buf = (uint8_t *)value;
    uint8_t tmp = buf[0];
    buf[0] = buf[1];
    buf[1] = tmp;
}

R11F_INTERNAL void flip4(uint32_t *value) {
    uint8_t *buf = (uint8_t *)value;
    uint8_t tmp = buf[0];
    buf[0] = buf[3];
    buf[3] = tmp;
    tmp = buf[1];
    buf[1] = buf[2];
    buf[2] = tmp;
}

R11F_INTERNAL uint16_t read_unaligned2(void* addr) {
    uint16_t value;
    memcpy(&value, addr, sizeof(uint16_t));
    return value;
}

R11F_INTERNAL void write_unaligned2(void* addr, uint16_t value) {
    memcpy(addr, &value, sizeof(uint16_t));
}

R11F_INTERNAL uint32_t read_unaligned4(void* addr) {
    uint32_t value;
    memcpy(&value, addr, sizeof(uint32_t));
    return value;
}

R11F_INTERNAL void write_unaligned4(void* addr, uint32_t value) {
    memcpy(addr, &value, sizeof(uint32_t));
}

R11F_INTERNAL uint64_t read_unaligned8(void* addr) {
    uint64_t value;
    memcpy(&value, addr, sizeof(uint64_t));
    return value;
}

R11F_INTERNAL void write_unaligned8(void* addr, uint64_t value) {
    memcpy(addr, &value, sizeof(uint64_t));
}

R11F_INTERNAL void flip2_unaligned(void* addr) {
    uint16_t value = read_unaligned2(addr);
    flip2(&value);
    write_unaligned2(addr, value);
}

R11F_INTERNAL void flip4_unaligned(void* addr) {
    uint32_t value = read_unaligned4(addr);
    flip4(&value);
    write_unaligned4(addr, value);
}
