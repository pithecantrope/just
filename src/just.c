#include "just.h"

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
        abort_if(data == NULL || len > ISIZE_MAX);
        s8 s = (s8){.data = alloc(a, u8, len), .len = (isize)len};
        memcpy(s.data, data, len);
        return s;
}

s8
s8dup(arena* a, s8 s) {
        s8 dup = (s8){.data = alloc(a, u8, s.len), .len = s.len};
        memcpy(dup.data, s.data, (usize)s.len);
        return dup;
}

s8
s8slice(arena* a, s8 s, isize start, isize stop, isize step) {
        assert(step != 0);
        start = CLAMP(0, start, s.len - 1);
        stop = (step > 0) ? CLAMP(0, stop, s.len) : CLAMP(-1, stop, s.len - 1);
        s8 slice = (s8){.data = alloc(a, u8, 0), .len = 0};
        for (isize i = start; (step > 0) ? (i < stop) : (i > stop); i += step) {
                *(u8*)alloc(a, u8) = s.data[i];
                ++slice.len;
        }
        return slice;
}

s8
s8repeat(arena* a, s8 s, isize n) {
        abort_if(s.len == 0);
        n = CLAMP(0, n, ISIZE_MAX / s.len);
        s8 repeat = {.data = alloc(a, u8, s.len * n), .len = s.len * n};
        for (isize i = 0; i < n; ++i) {
                memcpy(repeat.data + i * s.len, s.data, (usize)s.len);
        }
        return repeat;
}

s8
s8cat(arena* a, s8 s1, s8 s2) {
        s8 cat = {.data = alloc(a, u8, s1.len + s2.len), .len = s1.len + s2.len};
        memcpy(cat.data, s1.data, (usize)s1.len);
        memcpy(cat.data + s1.len, s2.data, (usize)s2.len);
        return cat;
}

s8
s8inject(arena* a, s8 s1, isize index, isize len, s8 s2) {
        index = CLAMP(0, index, s1.len);
        len = CLAMP(0, len, s1.len - index);
        s8 inject = {.data = alloc(a, u8, s1.len + s2.len - len), .len = s1.len + s2.len - len};
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
        index = CLAMP(0, index, s.len - 1);
        return (s8){.data = s.data + index, .len = CLAMP(0, len, s.len - index)};
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

indexes
s8findall(arena* a, s8 s, s8 sub) {
        indexes arr = {.data = alloc(a, isize, 0), .len = 0};
        if (s.len < sub.len) {
                return arr;
        }
        if (sub.len == 0) {
                alloc(a, isize, arr.len = s.len);
                for (isize i = 0; i < arr.len; ++i) {
                        arr.data[i] = i;
                }
                return arr;
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
                                ++arr.len;
                                break;
                        }
                }
                i += sub.len - 1 - last_occ[s.data[i + sub.len - 1]];
        }
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
