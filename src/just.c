#include "just.h"

// arena -------------------------------------------------------------------------------------------
arena*
arena_create(usize capacity) {
        assert(sizeof(arena) <= SIZE_MAX - capacity);
        arena* a = malloc(sizeof(arena) + capacity);
        assert(a != NULL);
        *a = (arena){.cap = capacity};
        return a;
}

void*
arena_alloc(arena* a, usize align, usize size, usize count) {
        assert(a != NULL && "Invalid arena");
        assert(size != 0 && "Invalid size");
        assert(IS_POW2(align) && "Invalid alignment");
        usize padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        assert(count <= (a->cap - a->used - padding) / size && "Increase arena capacity");
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}
