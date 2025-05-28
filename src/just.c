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
string_new(arena* a, const char* str, size_t len) {
        assert(str != NULL && "Invalid str");
        assert(len <= INT_MAX && "Invalid len");
        char* data = allocn(a, char, len);
        memcpy(data, str, len);
        return (string){.data = data, .len = (int)len};
}

char*
string_str(arena* a, string s) {
        char* str = allocn(a, char, s.len + 1);
        memcpy(str, s.data, (size_t)s.len);
        str[s.len] = '\0';
        return str;
}

string
string_cat(arena* a, string base, string s) {
        assert(base.len <= INT_MAX - s.len && "Result string is too large");
        if (base.data + base.len == s.data) {
                base.len += s.len;
                return base;
        }
        if (base.data + base.len == a->data + a->used) {
                memcpy(allocn(a, char, s.len), s.data, (size_t)s.len);
                base.len += s.len;
                return base;
        }
        char* data = allocn(a, char, base.len + s.len);
        memcpy(data, base.data, (size_t)base.len);
        memcpy(data + base.len, s.data, (size_t)s.len);
        return (string){.data = data, .len = base.len + s.len};
}

string
string_fit(arena* a, string s, int len) {
        assert(s.len > 0 && "Invalid s");
        assert(len > s.len && "Invalid len");
        char* data = allocn(a, char, len);
        for (int i = 0; i < len / s.len; ++i) {
                memcpy(data + i * s.len, s.data, (size_t)s.len);
        }
        memcpy(data + len - len % s.len, s.data, (size_t)(len % s.len));
        return (string){.data = data, .len = len};
}

string string_file(arena* a, const char* path);

string
string_sub(string s, int index, int len) {
        assert(ISIN(0, index, s.len) && "Invalid index");
        assert(ISIN(0, len, s.len - index) && "Invalid len");
        return (string){.data = s.data + index, .len = len - index};
}
