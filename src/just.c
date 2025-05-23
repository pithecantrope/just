#include "just.h"
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>

// arena -------------------------------------------------------------------------------------------
arena*
arena_create(size_t capacity) {
        arena* a = malloc(sizeof(arena));
        assert(a != NULL);
        *a = (arena){.cap = capacity, .data = malloc(capacity)};
        assert(a->data != NULL);
        return a;
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

// string ------------------------------------------------------------------------------------------
string
string_new(arena* a, const char* data, size_t len) {
        assert(data != NULL && "Invalid data");
        assert(len <= INT_MAX && "Invalid len");
        string s = {.data = allocn(a, char, len), .len = (int)len};
        memcpy(s.data, data, len);
        return s;
}

string
string_fmt(arena* a, const char* fmt, ...) {
        va_list args, copy;
        va_start(args, fmt);
        va_copy(copy, args);
        int len = vsnprintf(NULL, 0, fmt, copy);
        va_end(copy);
        assert(len >= 0 && "Invalid format");
        char* data = allocn(a, char, len);
        vsnprintf(data, (size_t)len + 1, fmt, args);
        va_end(args);
        return (string){.data = data, .len = len};
}

string
string_dup(arena* a, string s) {
        string res = {.data = allocn(a, char, s.len), .len = s.len};
        memcpy(res.data, s.data, (size_t)s.len);
        return res;
}

string
string_cat(arena* a, string base, string s) {
        assert(base.len <= INT_MAX - s.len && "Result string is too large");
        if (base.data + base.len == s.data && s.data + s.len <= a->data + a->used) {
                base.len += s.len;
                return base;
        }
        if (base.data + base.len != a->data + a->used) {
                base = string_dup(a, base);
        }
        base.len += string_dup(a, s).len;
        return base;
}

char*
string_z(arena* a, string s) {
        if (s.data + s.len != a->data + a->used) {
                s = string_dup(a, s);
        }
        alloc(a, char);
        s.data[s.len] = '\0';
        return s.data;
}

bool
string_istitle(string s) {
        for (int i = 0; i < s.len; ++i) {
                for (; i < s.len && !isalpha(s.data[i]); ++i) {}
                if (islower(s.data[i])) {
                        return false;
                }
                ++i;
                for (; i < s.len && isalpha(s.data[i]); ++i) {
                        if (isupper(s.data[i])) {
                                return false;
                        }
                }
        }
        return true;
}

string
string_capitalize(string s) {
        if (s.len > 0) {
                s.data[0] = (char)toupper(s.data[0]);
                for (int i = 1; i < s.len; ++i) {
                        s.data[i] = (char)tolower(s.data[i]);
                }
        }
        return s;
}

string
string_title(string s) {
        for (int i = 0; i < s.len; ++i) {
                for (; i < s.len && !isalpha(s.data[i]); ++i) {}
                s.data[i] = (char)toupper(s.data[i]);
                ++i;
                for (; i < s.len && isalpha(s.data[i]); ++i) {
                        s.data[i] = (char)tolower(s.data[i]);
                }
        }
        return s;
}
