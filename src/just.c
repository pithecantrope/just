#include "just.h"

// arena -------------------------------------------------------------------------------------------
arena*
arena_create(usize capacity) {
        JUST_ASSERT(sizeof(arena) <= USIZE_MAX - capacity);
        arena* a = malloc(sizeof(arena) + capacity);
        JUST_ASSERT(a != NULL);
        *a = (arena){.cap = capacity};
        return a;
}

void*
arena_alloc(arena* a, usize size, usize align, usize count) {
        JUST_ASSERT(a != NULL && "Invalid arena");
        JUST_ASSERT(size != 0 && "Invalid size");
        JUST_ASSERT(IS_POW2(align) && "Invalid alignment");
        usize padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        JUST_ASSERT(count <= (a->cap - a->used - padding) / size && "Increase arena capacity");
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}

// s8 ----------------------------------------------------------------------------------------------
s8
s8new(arena* a, const char* data, usize len) {
        JUST_ASSERT(data != NULL && "Invalid data");
        JUST_ASSERT(len <= ISIZE_MAX && "Invalid length");
        s8 s = (s8){.data = alloc(a, u8, len), .len = (isize)len};
        memcpy(s.data, data, len);
        return s;
}

s8
s8dup(arena* a, s8 s) {
        JUST_ASSERT(s.data != NULL && 0 < s.len && "Invalid string");
        s8 dup = (s8){.data = alloc(a, u8, s.len), .len = s.len};
        memcpy(dup.data, s.data, (usize)s.len);
        return dup;
}

s8
s8slice(arena* a, s8 s, isize start, isize stop, isize step) {
        JUST_ASSERT(s.data != NULL && 0 < s.len && "Invalid string");
        JUST_ASSERT(step != 0 && "Invalid step");
        bool n = step < 0;
        JUST_ASSERT(IS_IN(0, start, s.len - 1) && "Out of bounds");
        JUST_ASSERT(IS_IN(0 - n, stop, s.len - n) && "Out of bounds");
        isize len = (DIFF(start, stop) + ABS(step) - 1) / ABS(step);
        s8 slice = (s8){.data = alloc(a, u8, len), .len = len};
        for (isize i = 0, j = start; n ? (j > stop) : (j < stop); ++i, j += step) {
                slice.data[i] = s.data[j];
        }
        return slice;
}

s8
s8repeat(arena* a, s8 s, isize n) {
        JUST_ASSERT(s.data != NULL && 0 < s.len && "Invalid string");
        assert(IS_IN(0, n, ISIZE_MAX / s.len) && "Result string is too large");
        s8 repeat = {.data = alloc(a, u8, s.len * n), .len = s.len * n};
        for (isize i = 0; i < n; ++i) {
                memcpy(repeat.data + i * s.len, s.data, (usize)s.len);
        }
        return repeat;
}

s8
s8cat(arena* a, s8 s1, s8 s2) {
        JUST_ASSERT(s1.data != NULL && 0 < s1.len && "Invalid string");
        JUST_ASSERT(s2.data != NULL && 0 < s2.len && "Invalid string");
        JUST_ASSERT(s1.len <= ISIZE_MAX - s2.len && "Result string is too large");
        s8 cat = {.data = alloc(a, u8, s1.len + s2.len), .len = s1.len + s2.len};
        memcpy(cat.data, s1.data, (usize)s1.len);
        memcpy(cat.data + s1.len, s2.data, (usize)s2.len);
        return cat;
}

s8
s8inject(arena* a, s8 s1, isize index, isize len, s8 s2) {
        JUST_ASSERT(IS_IN(0, index, s1.len) && "Out of bounds");
        JUST_ASSERT(IS_IN(0, len, s1.len - index) && "Out of bounds");
        JUST_ASSERT(s1.len - len <= ISIZE_MAX - s2.len && "Result string is too large");
        s8 inject = {.data = alloc(a, u8, s1.len - len + s2.len), .len = s1.len - len + s2.len};
        memcpy(inject.data, s1.data, (usize)index);
        memcpy(inject.data + index, s2.data, (usize)s2.len);
        memcpy(inject.data + index + s2.len, s1.data + index + len, (usize)(s1.len - index - len));
        return inject;
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
        JUST_ASSERT(IS_IN(0, index, s.len) && "Out of bounds");
        JUST_ASSERT(IS_IN(0, len, s.len - index) && "Out of bounds");
        return (s8){.data = s.data + index, .len = len};
}

isize
s8find(s8 s, s8 sub) {
        assert(s.data && 0 < s.len && "Invalid string");
        assert(sub.data && IS_IN(1, sub.len, s.len) && "Invalid substring");
        isize last[U8ASCII]; // Horspool algorithm
        memset(last, -1, sizeof(isize) * U8ASCII);
        for (isize i = 0; i < sub.len - 1; ++i) {
                last[sub.data[i]] = i;
        }
        for (isize i = 0; i <= s.len - sub.len; i += sub.len - 1 - last[s.data[i + sub.len - 1]]) {
                for (isize j = sub.len - 1; sub.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                return i;
                        }
                }
        }
        return -1;
}

isize
s8count(s8 s, s8 sub) {
        assert(s.data && 0 < s.len && "Invalid string");
        assert(sub.data && IS_IN(1, sub.len, s.len) && "Invalid substring");
        isize last[U8ASCII], count = 0;
        memset(last, -1, sizeof(isize) * U8ASCII);
        for (isize i = 0; i < sub.len - 1; ++i) {
                last[sub.data[i]] = i;
        }
        for (isize i = 0; i <= s.len - sub.len; i += sub.len - 1 - last[s.data[i + sub.len - 1]]) {
                for (isize j = sub.len - 1; sub.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                ++count;
                                break;
                        }
                }
        }
        return count;
}

isizes
s8findall(arena* a, s8 s, s8 sub) {
        assert(s.data && 0 < s.len && "Invalid string");
        assert(IS_IN(1, sub.len, s.len) && "Invalid substring");
        isizes arr = {.data = alloc(a, isize, 0), .len = 0};
        isize last[U8ASCII];
        memset(last, -1, sizeof(isize) * U8ASCII);
        for (isize i = 0; i < sub.len - 1; ++i) {
                last[sub.data[i]] = i;
        }
        for (isize i = 0; i <= s.len - sub.len; i += sub.len - 1 - last[s.data[i + sub.len - 1]]) {
                for (isize j = sub.len - 1; sub.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                *(isize*)alloc(a, isize) = i;
                                ++arr.len;
                                break;
                        }
                }
        }
        return arr;
}

s8s
s8split(arena* a, s8 s, s8 sep) {
        assert(s.data && 0 < s.len && "Invalid string");
        assert(IS_IN(1, sep.len, s.len) && "Invalid separator");
        s8s arr = {.data = alloc(a, s8, 0), .len = 0};
        isize last[U8ASCII], prev = 0;
        memset(last, -1, sizeof(isize) * U8ASCII);
        for (isize i = 0; i < sep.len - 1; ++i) {
                last[sep.data[i]] = i;
        }
        for (isize i = 0; i <= s.len - sep.len; i += sep.len - 1 - last[s.data[i + sep.len - 1]]) {
                for (isize j = sep.len - 1; sep.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                *(s8*)alloc(a, s8) = (s8){.data = s.data + prev, .len = i - prev};
                                ++arr.len;
                                prev = i + sep.len;
                                break;
                        }
                }
        }
        *(s8*)alloc(a, s8) = (s8){.data = s.data + prev, .len = s.len - prev};
        ++arr.len;
        return arr;
}

bool
s8is_title(s8 s) {
        for (isize i = 0; i < s.len; ++i) {
                for (; i < s.len && !u8is_alpha(s.data[i]); ++i) {}
                if (u8is_lower(s.data[i++])) {
                        return false;
                }
                for (; i < s.len && u8is_alpha(s.data[i]); ++i) {
                        if (u8is_upper(s.data[i])) {
                                return false;
                        }
                }
        }
        return true;
}

s8
s8capitalize(s8 s) {
        if (s.len > 0) {
                s.data[0] = u8upper(s.data[0]);
                for (isize i = 1; i < s.len; ++i) {
                        s.data[i] = u8lower(s.data[i]);
                }
        }
        return s;
}

s8
s8title(s8 s) {
        for (isize i = 0; i < s.len; ++i) {
                for (; i < s.len && !u8is_alpha(s.data[i]); ++i) {}
                s.data[i] = u8upper(s.data[i]);
                ++i;
                for (; i < s.len && u8is_alpha(s.data[i]); ++i) {
                        s.data[i] = u8lower(s.data[i]);
                }
        }
        return s;
}
