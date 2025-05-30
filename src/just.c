#include "just.h"
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>

// Arena -------------------------------------------------------------------------------------------
arena*
arena_create(size_t capacity) {
        arena* a = malloc(sizeof(arena));
        assert(a != NULL);
        *a = (arena){.cap = capacity, .data = malloc(capacity)};
        assert(a->data != NULL);
        return a;
}

inline void
arena_destroy(arena* a) {
        assert(a != NULL && "Invalid arena");
        free(a->data);
        free(a);
}

inline void
arena_reset(arena* a) {
        assert(a != NULL && "Invalid arena");
        a->used = 0;
}

void*
arena_alloc(arena* a, size_t align, size_t size, size_t count) {
        assert(a != NULL && "Invalid arena");
        assert(ISPOW2(align) && "Invalid alignment");
        assert(size != 0 && "Invalid size");
        size_t padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        assert(count <= (a->cap - a->used - padding) / size && "Increase arena capacity");
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}

// String ------------------------------------------------------------------------------------------

// Vector ------------------------------------------------------------------------------------------

// Hashmap -----------------------------------------------------------------------------------------
