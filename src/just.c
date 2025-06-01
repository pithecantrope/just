#include "just.h"
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>

// Arena -------------------------------------------------------------------------------------------
arena*
arena_create(size_t capacity) {
        arena* a = malloc(sizeof(arena));
        assert(a != NULL);
        *a = (arena){.cap = capacity, .data = malloc(capacity)};
        assert(a->data != NULL);
        return a;
}

void
arena_destroy(arena* a) {
        assert(a != NULL && "Invalid arena");
        free(a->data);
        free(a);
}

void
arena_reset(arena* a) {
        assert(a != NULL && "Invalid arena");
        a->used = 0;
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

// String ------------------------------------------------------------------------------------------
string
string_dup(arena* a, string s) {
        string res = (string){.data = allocn(a, char, s.len), s.len};
        memcpy(res.data, s.data, (size_t)s.len);
        return res;
}

string
string_fmt(arena* a, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        int len = vsnprintf(NULL, 0, fmt, args);
        va_end(args);
        assert(len >= 0 && "Invalid format");

        va_start(args, fmt);
        string res = (string){.data = allocn(a, char, len), .len = len};
        vsnprintf(res.data, (size_t)len + 1, fmt, args); // I know
        va_end(args);
        return res;
}

string
string_new(arena* a, const char* null, size_t len) {
        assert(null != NULL && "Invalid null-terminated string");
        assert(len <= INT_MAX && "Invalid len");
        string res = {.data = allocn(a, char, len), .len = (int)len};
        memcpy(res.data, null, len);
        return res;
}

string
string_file(arena* a, const char* path) {
        FILE* file = fopen(path,
#if defined(__linux__)
                           "rbe"
#else
                           "rb"
#endif
        );
        assert(file != NULL && "Invalid path");

        int err = fseek(file, 0, SEEK_END); // Non-standard, but widely supported
        assert(err == 0);
        long len = ftell(file);
        assert(len > 0);
        assert(len <= INT_MAX && "File is too large");
        rewind(file);

        string res = (string){.data = allocn(a, char, len), .len = (int)len};
        size_t read = fread(res.data, sizeof(char), (size_t)len, file);
        assert((size_t)len == read);
        err = fclose(file);
        assert(err == 0);
        return res;
}

char*
string_null(arena* a, string mut_s) {
        if (mut_s.data + mut_s.len != a->data + a->used) {
                mut_s = string_dup(a, mut_s);
        }
        alloc(a, char);
        mut_s.data[mut_s.len] = '\0';
        return mut_s.data;
}

string
string_cat(arena* a, string mut_base, string mut_s) {
        assert(mut_base.len <= INT_MAX - mut_s.len && "Result string is too large");
        if (mut_base.data + mut_base.len == mut_s.data) {
                mut_base.len += mut_s.len;
                return mut_base;
        }
        if (mut_base.data + mut_base.len != a->data + a->used) {
                mut_base = string_dup(a, mut_base);
        }
        mut_base.len += string_dup(a, mut_s).len;
        return mut_base;
}

string
string_insert(arena* a, string base, int index, string s) {
        assert(ISIN(0, index, base.len) && "Invalid index");
        assert(base.len <= INT_MAX - s.len && "Result string is too large");
        string res = {.data = allocn(a, char, base.len + s.len), .len = base.len + s.len};
        memcpy(res.data, base.data, (size_t)index);
        memcpy(res.data + index, s.data, (size_t)s.len);
        memcpy(res.data + index + s.len, base.data + index, (size_t)(base.len - index));
        return res;
}

bool
string_eq(string s1, string s2) {
        return s1.len == s2.len && memcmp(s1.data, s2.data, (size_t)s1.len) == 0;
}

// Vector ------------------------------------------------------------------------------------------

// Hashmap -----------------------------------------------------------------------------------------
