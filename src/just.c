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
string_cat(arena* a, string mut_base, string mut_s) {
        assert(mut_base.len <= INT_MAX - mut_s.len && "Result string is too large");
        if (mut_base.data + mut_base.len == mut_s.data
            && mut_s.data + mut_s.len <= a->data + a->used) {
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
string_inject(arena* a, string base, int index, int len, string s) {
        assert(ISIN(0, index, base.len) && "Out of bounds");
        assert(ISIN(0, len, base.len - index) && "Out of bounds");
        assert(base.len - len <= INT_MAX - s.len && "Result string is too large");
        int res_len = base.len - len + s.len;
        string res = {.data = allocn(a, char, res_len), .len = res_len};
        memcpy(res.data, base.data, (size_t)index);
        memcpy(res.data + index, s.data, (size_t)s.len);
        memcpy(res.data + index + s.len, base.data + index + len, (size_t)(base.len - index - len));
        return res;
}

string
string_ljust(arena* a, string mut_s, int width, char fill) {
        assert(mut_s.len <= width && "Invalid width");
        if (mut_s.data + mut_s.len != a->data + a->used) {
                mut_s = string_dup(a, mut_s);
        }
        allocn(a, char, width - mut_s.len);
        memset(mut_s.data + mut_s.len, fill, (size_t)(width - mut_s.len));
        mut_s.len = width;
        return mut_s;
}

string
string_rjust(arena* a, string s, int width, char fill) {
        assert(s.len <= width && "Invalid width");
        char* data = allocn(a, char, width);
        memset(data, fill, (size_t)(width - s.len));
        memcpy(data + (width - s.len), s.data, (size_t)s.len);
        return (string){.data = data, .len = width};
}

string
string_center(arena* a, string s, int width, char fill) {
        assert(s.len <= width && "Invalid width");
        char* data = allocn(a, char, width);
        int left = (width - s.len) >> 1;
        memset(data, fill, (size_t)left);
        memcpy(data + left, s.data, (size_t)s.len);
        memset(data + left + s.len, fill, (size_t)(width - left - s.len));
        return (string){.data = data, .len = width};
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

        int err = fseek(file, 0, SEEK_END); // Non-standard
        assert(err == 0);
        long len = ftell(file);
        assert(len != -1L && len != 0);
        assert(len <= INT_MAX && "File is too large");
        rewind(file);

        char* data = allocn(a, char, len);
        size_t read = fread(data, sizeof(char), (size_t)len, file);
        assert((size_t)len == read);
        err = fclose(file);
        assert(err == 0);
        return (string){.data = data, .len = (int)len};
}

char*
string_z(arena* a, string mut_s) {
        if (mut_s.data + mut_s.len != a->data + a->used) {
                mut_s = string_dup(a, mut_s);
        }
        alloc(a, char);
        mut_s.data[mut_s.len] = '\0';
        return mut_s.data;
}

int
string_cmp(string s1, string s2) {
        int cmp = memcmp(s1.data, s2.data, (size_t)MIN(s1.len, s2.len));
        return cmp ? (cmp < 0 ? -1 : 1) : (s1.len < s2.len ? -1 : (s1.len > s2.len ? 1 : 0));
}

int
string_icmp(string s1, string s2) {
        for (int i = 0; i < MIN(s1.len, s2.len); ++i) {
                char c1 = (char)toupper(s1.data[i]), c2 = (char)toupper(s2.data[i]);
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
        for (int i = 0; i < s1.len; ++i) {
                if (toupper(s1.data[i]) != toupper(s2.data[i])) {
                        return false;
                }
        }
        return true;
}

bool
string_in(string sub, string s) {
        if (sub.len > s.len) {
                return false;
        }
        if (sub.len == 0) {
                return true;
        }
        int last[UCHAR_MAX + 1]; // Horspool algorithm
        memset(last, -1, sizeof(last));
        for (int i = 0; i < sub.len - 1; ++i) {
                last[(unsigned char)sub.data[i]] = i;
        }
        for (int i = 0; i <= s.len - sub.len;
             i += sub.len - 1 - last[(unsigned char)s.data[i + sub.len - 1]]) {
                for (int j = sub.len - 1; sub.data[j] == s.data[j + i];) {
                        if (--j == -1) {
                                return true;
                        }
                }
        }
        return false;
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
string_capitalize(string mut_s) {
        if (mut_s.len > 0) {
                mut_s.data[0] = (char)toupper(mut_s.data[0]);
                for (int i = 1; i < mut_s.len; ++i) {
                        mut_s.data[i] = (char)tolower(mut_s.data[i]);
                }
        }
        return mut_s;
}

string
string_title(string mut_s) {
        for (int i = 0; i < mut_s.len; ++i) {
                for (; i < mut_s.len && !isalpha(mut_s.data[i]); ++i) {}
                mut_s.data[i] = (char)toupper(mut_s.data[i]);
                ++i;
                for (; i < mut_s.len && isalpha(mut_s.data[i]); ++i) {
                        mut_s.data[i] = (char)tolower(mut_s.data[i]);
                }
        }
        return mut_s;
}
