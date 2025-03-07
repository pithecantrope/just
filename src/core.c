#include "core.h"

// arena -------------------------------------------------------------------------------------------
arena*
arena_new(usize capacity) {
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
bool
s8is_title(const s8 s PTR) {
        TODO(s);
        return true;
}

s8*
s8title(s8 s PTR) {
        return s;
}

i32
s8cmp(const s8 s1 PTR, const s8 s2 PTR) {
        if (s1->len != s2->len) {
                return s1->len < s2->len ? -1 : 1;
        }
        return memcmp(s1->data, s2->data, (size_t)s1->len);
}

inline bool
s8eq(const s8 s1 PTR, const s8 s2 PTR) {
        return s8cmp(s1, s2) == 0;
}

bool
s8starts_with(const s8 s PTR, const s8 prefix PTR) {
        return (s->len >= prefix->len) && memcmp(s->data, prefix->data, (size_t)prefix->len) == 0;
}

bool
s8ends_with(const s8 s PTR, const s8 suffix PTR) {
        return (s->len >= suffix->len)
               && memcmp(s->data + (s->len - suffix->len), suffix->data, (size_t)suffix->len) == 0;
}

// Horspool algorithm
isize
s8find(const s8 s PTR, const s8 sub PTR) {
        if (s->len < sub->len) {
                return -1;
        }
        if (sub->len == 0) {
                return 0;
        }
        isize last_occ[U8ALPHABET];
        memset(last_occ, -1, sizeof(isize) * U8ALPHABET);
        for (isize i = 0; i < sub->len - 1; ++i) {
                last_occ[sub->data[i]] = i;
        }

        for (isize i = 0; i <= s->len - sub->len;) {
                for (isize j = sub->len - 1; sub->data[j] == s->data[j + i];) {
                        if (--j == -1) {
                                return i;
                        }
                }
                i += sub->len - 1 - last_occ[s->data[i + sub->len - 1]];
        }
        return -1;
}

isize
s8count(const s8 s PTR, const s8 sub PTR) {
        if (s->len < sub->len) {
                return 0;
        }
        if (sub->len == 0) {
                return s->len;
        }
        isize count = 0;
        isize last_occ[U8ALPHABET];
        memset(last_occ, -1, sizeof(isize) * U8ALPHABET);
        for (isize i = 0; i < sub->len - 1; ++i) {
                last_occ[sub->data[i]] = i;
        }

        for (isize i = 0; i <= s->len - sub->len;) {
                for (isize j = sub->len - 1; sub->data[j] == s->data[j + i];) {
                        if (--j == -1) {
                                ++count;
                                break;
                        }
                }
                i += sub->len - 1 - last_occ[s->data[i + sub->len - 1]];
        }
        return count;
}
