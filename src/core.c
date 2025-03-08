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
// bool
// s8is_title(const s8 s PTR) {
//         for (isize i = 0; i < s->len; ++i) {
//                 for (; i < s->len && !u8is_alpha(s->data[i]); ++i) {}
//                 if (u8is_lower(s->data[i])) {
//                         return false;
//                 }
//                 for (; i < s->len && u8is_alpha(s->data[i]); ++i) {
//                         if (u8is_upper(s->data[i])) {
//                                 return false;
//                         }
//                 }
//         }
//         return true;
// }

// s8*
// s8capitalize(s8 s PTR) {
//         if (s->len > 0) {
//                 s->data[0] = u8upper(s->data[0]);
//                 for (isize i = 1; i < s->len; ++i) {
//                         s->data[i] = u8lower(s->data[i]);
//                 }
//         }
//         return s;
// }

s8*
s8title(s8 s PTR) {
        return s;
}

s8*
s8cstr(arena* a, const char data PTR, size_t len) {
        assert(len <= ISIZE_MAX);
        s8* s = alloc(a, s8);
        *s = (s8){.data = alloc(a, u8, len), .len = (isize)len};
        memcpy(s->data, data, sizeof(*s->data) * len);
        return s;
}

s8*
s8copy(arena* a, const s8 s PTR) {
        s8* copy = alloc(a, s8);
        *copy = (s8){.data = alloc(a, u8, (size_t)s->len), .len = s->len};
        memcpy(copy->data, s, sizeof(*s->data) * (size_t)s->len);
        return copy;
}

int
s8cmp(const s8 s1 PTR, const s8 s2 PTR) {
        if (s1->len != s2->len) {
                return s1->len < s2->len ? -1 : 1;
        }
        int cmp = memcmp(s1->data, s2->data, (size_t)s1->len);
        return (cmp > 0) - (cmp < 0);
}

int
s8icmp(const s8 s1 PTR, const s8 s2 PTR) {
        if (s1->len != s2->len) {
                return s1->len < s2->len ? -1 : 1;
        }
        for (isize i = 0; i < s1->len; ++i) {
                u8 u1 = u8upper(s1->data[i]), u2 = u8upper(s2->data[i]);
                if (u1 != u2) {
                        return u1 < u2 ? -1 : 1;
                }
        }
        return 0;
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
s8rfind(const s8 s PTR, const s8 sub PTR) {
        TODO(s, sub);
        return 0;
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
