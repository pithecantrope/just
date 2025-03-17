#include "core.h"

// arena -------------------------------------------------------------------------------------------
arena*
arena_create(usize capacity) {
        abort_if(sizeof(arena) > USIZE_MAX - capacity);
        arena* a = malloc(sizeof(arena) + capacity);
        abort_if(a == NULL);
        *a = (arena){.cap = capacity};
        return a;
}

void*
arena_alloc(arena* a, usize size, usize align, usize count) {
        assert(a != NULL && IS_POW2(align));
        usize padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        abort_if(size == 0 || count > (a->cap - a->used - padding) / size);
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}

// s8 ----------------------------------------------------------------------------------------------
s8
s8new(arena* a, const char* data, usize len) {
        assert(data != NULL && len <= ISIZE_MAX);
        s8 s = (s8){.data = alloc(a, u8, len), .len = (isize)len};
        memcpy((u8*)s.data, data, len);
        return s;
}

s8
s8dup(arena* a, s8 s) {
        s8 copy = (s8){.data = alloc(a, u8, (usize)s.len), .len = s.len};
        memcpy((u8*)copy.data, s.data, (usize)s.len);
        return copy;
}

i32
s8cmp(s8 s1, s8 s2) {
        if (s1.len != s2.len) {
                return s1.len < s2.len ? -1 : 1;
        }
        i32 cmp = memcmp(s1.data, s2.data, (usize)s1.len);
        return (cmp > 0) - (cmp < 0);
}

i32
s8icmp(s8 s1, s8 s2) {
        if (s1.len != s2.len) {
                return s1.len < s2.len ? -1 : 1;
        }
        for (isize i = 0; i < s1.len; ++i) {
                 u8 u1 = u8lower(s1.data[i]), u2 = u8lower(s2.data[i]);
                 if (u1 != u2) {
                         return u1 < u2 ? -1 : 1;
                 }
         }
        return 0;
}
