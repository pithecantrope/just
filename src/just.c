#include "just.h"
#include <stdarg.h>

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
        assert(ISPOW2(align) && "Invalid alignment");
        assert(size != 0 && "Invalid size");
        u64 padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        assert(count <= (a->cap - a->used - padding) / size && "Increase arena capacity");
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}

// string ------------------------------------------------------------------------------------------
string
string_new(arena* a, const char* data, size_t len) {
        assert(data != NULL && "Invalid data");
        assert(len <= INT32_MAX && "Invalid len");
        string s = {.data = alloc(a, ascii, len), .len = (i32)len};
        memcpy(s.data, data, len);
        return s;
}

string
string_fmt(arena* a, const char* fmt, ...) {
        va_list args, copy;
        va_start(args, fmt);
        va_copy(copy, args);
        i32 len = vsnprintf(NULL, 0, fmt, copy);
        va_end(copy);
        assert(len >= 0 && "Invalid format");
        ascii* data = alloc(a, ascii, len);
        vsnprintf((char*)data, (size_t)len + 1, fmt, args);
        va_end(args);
        return (string){.data = data, .len = len};
}

string
string_dup(arena* a, string s) {
        string res = {.data = alloc(a, ascii, s.len), .len = s.len};
        memcpy(res.data, s.data, (size_t)s.len);
        return res;
}

string
string_cat(arena* a, string self, string s) {
        assert(self.len <= INT32_MAX - s.len && "Result string is too large");
        if ((byte*)(self.data + self.len) == (byte*)s.data
            && (byte*)(s.data + s.len) <= a->data + a->used) {
                self.len += s.len;
        } else {
                if ((byte*)(self.data + self.len) != a->data + a->used) {
                        self = string_dup(a, self);
                }
                self.len += string_dup(a, s).len;
        }
        return self;
}

string
string_inject(arena* a, string self, i32 index, i32 len, string s) {
        assert(ISIN(0, index, self.len) && "Out of bounds");
        assert(ISIN(0, len, self.len - index) && "Out of bounds");
        assert(self.len - len <= INT32_MAX - s.len && "Result string is too large");
        i32 res_len = self.len - len + s.len;
        string res = {.data = alloc(a, ascii, res_len), .len = res_len};
        memcpy(res.data, self.data, (size_t)index);
        memcpy(res.data + index, s.data, (size_t)s.len);
        memcpy(res.data + index + s.len, self.data + index + len, (size_t)(self.len - index - len));
        return res;
}

int
string_cmp(string s1, string s2) {
        int cmp = memcmp(s1.data, s2.data, (size_t)MIN(s1.len, s2.len));
        return cmp ? (cmp < 0 ? -1 : 1) : (s1.len < s2.len ? -1 : (s1.len > s2.len ? 1 : 0));
}

int
string_icmp(string s1, string s2) {
        for (i32 i = 0; i < MIN(s1.len, s2.len); ++i) {
                ascii c1 = ascii_upper(s1.data[i]), c2 = ascii_upper(s2.data[i]);
                if (c1 != c2) {
                        return c1 < c2 ? -1 : 1;
                }
        }
        return s1.len < s2.len ? -1 : (s1.len > s2.len ? 1 : 0);
}

bool
string_ieq(string s1, string s2) {
        if (s1.len != s2.len) {
                return false;
        }
        for (i32 i = 0; i < s1.len; ++i) {
                if (ascii_upper(s1.data[i]) != ascii_upper(s2.data[i])) {
                        return false;
                }
        }
        return true;
}
