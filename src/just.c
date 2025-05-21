#include "just.h"
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>

// arena -------------------------------------------------------------------------------------------
arena*
arena_create(size_t capacity) {
        assert(sizeof(arena) <= SIZE_MAX - capacity);
        arena* a = malloc(sizeof(arena) + capacity);
        assert(a != NULL);
        *a = (arena){.cap = capacity};
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
s8capitalize(string s) {
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
