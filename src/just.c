#include "just.h"
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
// #include <sys/mman.h>
// If you insist on this route, on 64-bit hosts you can reserve a gigantic
// virtual address space and gradually commit memory as needed. On Linux that
// means leaning on overcommit by allocating the largest arena possible at
// startup, which will automatically commit through use. Use MADV_FREE to
// decommit. On Windows, VirtualAlloc handles reserve and commit separately.
// In addition to the allocation offset, you need a commit offset. Then expand
// the committed region ahead of the allocation offset as it grows. If you
// ever manually reset the allocation offset, you could decommit as well, or at
// least MEM_RESET. At some point commit may fail, which should then trigger the
// out-of-memory policy, but the system is probably in poor shape by that point
// — i.e. use an abort policy to release it all quickly.

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

void
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
