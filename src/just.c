#include "just.h"

// arena -------------------------------------------------------------------------------------------
arena*
arena_create(u64 capacity) {
        assert(sizeof(arena) <= UINT64_MAX - capacity);
        arena* a = malloc(sizeof(arena) + capacity);
        assert(a != NULL);
        *a = (arena){.cap = capacity};
        return a;
}

void*
arena_alloc(arena* a, u64 align, u64 size, u64 count) {
        assert(a != NULL && "Invalid arena");
        assert(size != 0 && "Invalid size");
        assert(IS_POW2(align) && "Invalid alignment");
        u64 padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        assert(count <= (a->cap - a->used - padding) / size && "Increase arena capacity");
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}
