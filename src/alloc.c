#include "memstat.h"
#include "alloc.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

static _Atomic(size_t) g_heap_mem_used = 0;
static _Atomic(size_t) g_alloc_count = 0;
static _Atomic(size_t) g_dealloc_count = 0;
static _Atomic(size_t) g_fail_count = 0;

R11F_EXPORT r11f_memstat_t r11f_memstat_get(void) {
    r11f_memstat_t ret = {
        .heap_mem_used = atomic_load(&g_heap_mem_used),
        .alloc_count = atomic_load(&g_alloc_count),
        .dealloc_count = atomic_load(&g_dealloc_count),
        .fail_count = atomic_load(&g_fail_count),
    };
    return ret;
}

R11F_EXPORT void r11f_memstat_clear(void) {
    atomic_store(&g_heap_mem_used, 0);
    atomic_store(&g_alloc_count, 0);
    atomic_store(&g_dealloc_count, 0);
    atomic_store(&g_fail_count, 0);
}

R11F_EXPORT void* r11f_alloc(size_t size) {
    void *ret = malloc(size + sizeof(size_t));
    *(size_t*)ret = size;

    if (ret) {
        atomic_fetch_add(&g_heap_mem_used, size);
        atomic_fetch_add(&g_alloc_count, 1);
    }
    else {
        atomic_fetch_add(&g_fail_count, 1);
    }

    return (uint8_t*)ret + sizeof(size_t);
}

R11F_EXPORT void* r11f_alloc_zeroed(size_t size) {
    void *ret = calloc(1, size + sizeof(size_t));
    *(size_t*)ret = size;

    if (ret) {
        atomic_fetch_add(&g_heap_mem_used, size);
        atomic_fetch_add(&g_alloc_count, 1);
    }
    else {
        atomic_fetch_add(&g_fail_count, 1);
    }

    return (uint8_t*)ret + sizeof(size_t);
}

R11F_EXPORT void r11f_free(void *ptr) {
    if (ptr) {
        size_t size = *((size_t*)((uint8_t*)ptr - sizeof(size_t)));
        atomic_fetch_sub(&g_heap_mem_used, size);
        atomic_fetch_add(&g_dealloc_count, 1);
        free((uint8_t*)ptr - sizeof(size_t));
    }
}
