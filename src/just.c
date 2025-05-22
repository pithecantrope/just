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
string_dup(arena* a, string s) {
        string res = {.data = allocn(a, char, s.len), .len = s.len};
        memcpy(res.data, s.data, (size_t)s.len);
        return res;
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
