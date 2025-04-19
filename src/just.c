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
        assert(ISPOW2(align) && "Invalid alignment");
        u64 padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        assert(count <= (a->cap - a->used - padding) / size && "Increase arena capacity");
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}

// string ------------------------------------------------------------------------------------------
string
string_newlen(arena* a, const char* data, size_t len) {
        assert(data != NULL && "Invalid data");
        assert(len <= INT32_MAX && "Invalid len");
        string s = {.data = alloc(a, ascii, len), .len = (i32)len};
        memcpy(s.data, data, len);
        return s;
}

string
string_dup(arena* a, string s) {
        string dup = {.data = alloc(a, ascii, (u64)s.len), .len = s.len};
        memcpy(dup.data, s.data, (size_t)s.len);
        return dup;
}
