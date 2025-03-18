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
        s8 copy = (s8){.data = alloc(a, u8, s.len), .len = s.len};
        memcpy((u8*)copy.data, s.data, (usize)s.len);
        return copy;
}

s8
s8slice(arena* a, s8 s, isize start, isize stop, isize step) {
        assert(step != 0);
        start = (step > 0) ? MAX(start, 0) : MIN(start, s.len - 1);
        stop = (step > 0) ? MIN(stop, s.len) : MAX(stop, -1);

        u8* data = alloc(a, u8, 0);
        isize len = 0;
        for (isize i = start; (step > 0) ? (i < stop) : (i > stop); i += step) {
                *(u8*)alloc(a, u8) = s.data[i];
                ++len;
        }
        return (s8){.data = data, .len = len};
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

s8
s8span(s8 s, isize index, isize len) {
        TODO(s, index, len);
        return s;
}

// Horspool algorithm
isize
s8find(s8 s, s8 sub) {
        if (s.len < sub.len) {
                return -1;
        }
        if (sub.len == 0) {
                return 0;
        }
        isize last_occ[U8ASCII];
        memset(last_occ, -1, sizeof(isize) * U8ASCII);
        for (isize i = 0; i < sub.len - 1; ++i) {
                last_occ[sub.data[i]] = i;
        }

        for (isize i = 0; i <= s.len - sub.len;) {
                for (isize j = sub.len - 1; sub.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                return i;
                        }
                }
                i += sub.len - 1 - last_occ[s.data[i + sub.len - 1]];
        }
        return -1;
}

isize
s8count(s8 s, s8 sub) {
        if (s.len < sub.len) {
                return 0;
        }
        if (sub.len == 0) {
                return s.len;
        }
        isize last_occ[U8ASCII];
        memset(last_occ, -1, sizeof(isize) * U8ASCII);
        for (isize i = 0; i < sub.len - 1; ++i) {
                last_occ[sub.data[i]] = i;
        }

        isize count = 0;
        for (isize i = 0; i <= s.len - sub.len;) {
                for (isize j = sub.len - 1; sub.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                ++count;
                                break;
                        }
                }
                i += sub.len - 1 - last_occ[s.data[i + sub.len - 1]];
        }
        return count;
}

isizes
s8findall(arena* a, s8 s, s8 sub) {
        isizes index = {.data = alloc(a, isize, 0), .len = 0};
        if (s.len < sub.len) {
                return index;
        }
        if (sub.len == 0) {
                alloc(a, isize, s.len);
                index.len = s.len;
                for (isize i = 0; i < index.len; ++i) {
                        index.data[i] = i;
                }
                return index;
        }
        isize last_occ[U8ASCII];
        memset(last_occ, -1, sizeof(isize) * U8ASCII);
        for (isize i = 0; i < sub.len - 1; ++i) {
                last_occ[sub.data[i]] = i;
        }

        for (isize i = 0; i <= s.len - sub.len;) {
                for (isize j = sub.len - 1; sub.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                *(isize*)alloc(a, isize) = i;
                                ++index.len;
                                break;
                        }
                }
                i += sub.len - 1 - last_occ[s.data[i + sub.len - 1]];
        }
        return index;
}
