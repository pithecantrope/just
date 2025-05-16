#include "just.h"

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
string_dup(arena* a, string s) {
        string dup = {.data = alloc(a, ascii, s.len), .len = s.len};
        memcpy(dup.data, s.data, (size_t)s.len);
        return dup;
}

string
string_cat(arena* a, string head, string tail) {
        assert(head.len <= INT32_MAX - tail.len && "Result string is too large");
        if ((byte*)(head.data + head.len) != a->data + a->used) {
                head = string_dup(a, head);
        }
        head.len += string_dup(a, tail).len;
        return head;
}

string
string_inject(arena* a, string base, i32 index, i32 len, string inject) {
        assert(ISIN(0, index, base.len) && "Out of bounds");
        assert(ISIN(0, len, base.len - index) && "Out of bounds");
        assert(base.len - len <= INT32_MAX - inject.len && "Result string is too large");
        i32 result_len = base.len - len + inject.len;
        string result = {.data = alloc(a, ascii, result_len), .len = result_len};
        memcpy(result.data, base.data, (size_t)index);
        memcpy(result.data + index, inject.data, (size_t)inject.len);
        memcpy(result.data + index + inject.len, base.data + index + len,
               (size_t)(base.len - index - len));
        return result;
}

int string_cmp(string s1, string s2);
int string_icmp(string s1, string s2);

int
string_lcmp(string s1, string s2) {
        if (s1.len != s2.len) {
                return s1.len < s2.len ? -1 : 1;
        }
        int cmp = memcmp(s1.data, s2.data, (size_t)s1.len);
        return (cmp > 0) - (cmp < 0);
}

int
string_ilcmp(string s1, string s2) {
        if (s1.len != s2.len) {
                return s1.len < s2.len ? -1 : 1;
        }
        for (i32 i = 0; i < s1.len; ++i) {
                ascii u1 = ascii_upper(s1.data[i]), u2 = ascii_upper(s2.data[i]);
                if (u1 != u2) {
                        return u1 < u2 ? -1 : 1;
                }
        }
        return 0;
}
