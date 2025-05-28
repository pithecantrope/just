#include "just.h"
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>

// arena -------------------------------------------------------------------------------------------
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

inline arena_savepoint
arena_save(arena* a) {
        assert(a != NULL && "Invalid arena");
        return (arena_savepoint){.arena = a, .used = a->used};
}

inline void
arena_restore(arena_savepoint save) {
        assert(save.arena != NULL && "Invalid savepoint");
        save.arena->used = save.used;
}

// string ------------------------------------------------------------------------------------------
