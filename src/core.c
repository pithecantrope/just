#include "core.h"

// arena -------------------------------------------------------------------------------------------
arena_region*
new_region(usize capacity) {
        abort_if(sizeof(arena_region) > USIZE_MAX - capacity);
        arena_region* region = malloc(sizeof(arena_region) + capacity);
        abort_if(region == NULL);
        *region = (arena_region){.total = capacity};
        return region;
}

void*
arena_alloc_aligned(arena a PTR, usize size, usize align, usize count) {
        assert(IS_POW2(align));
        abort_if(size == 0 || count > USIZE_MAX / size);
        usize capacity = size * count;
        if (a->beg == NULL) {
                assert(a->end == NULL);
                a->beg = a->end = new_region(MAX(capacity, ARENA_REGION_CAPACITY));
        }
        arena_region* r;
        for (r = a->beg;
             r != NULL
             && capacity > r->total - r->used - (-(uintptr_t)(r->data + r->used) & (align - 1));
             r = r->next) {}

        if (r == NULL) {
                r = a->end = a->end->next = new_region(MAX(capacity, ARENA_REGION_CAPACITY));
        }
        usize padding = -(uintptr_t)(r->data + r->used) & (align - 1);
        void* ptr = r->data + r->used + padding;
        r->used += padding + capacity;
        return ptr;
}

void
arena_free(arena a PTR) {
        for (arena_region *next, *r = a->beg; r != NULL; r = next) {
                next = r->next;
                free(r);
        }
        a->beg = a->end = NULL;
}

void
arena_reset(arena a PTR) {
        for (arena_region* r = a->beg; r != NULL; r = r->next) {
                r->used = 0;
        }
}

// s8 ----------------------------------------------------------------------------------------------
i32
s8cmp(const s8 s1 PTR, const s8 s2 PTR) {
        if (s1->len != s2->len) {
                return s1->len < s2->len ? -1 : 1;
        }
        return memcmp(s1->data, s2->data, (size_t)s1->len);
}

inline bool
s8eq(const s8 s1 PTR, const s8 s2 PTR) {
        return s8cmp(s1, s2) == 0;
}

bool
s8starts_with(const s8 s PTR, const s8 prefix PTR) {
        return (s->len >= prefix->len) && memcmp(s->data, prefix->data, (size_t)prefix->len) == 0;
}

bool
s8ends_with(const s8 s PTR, const s8 suffix PTR) {
        return (s->len >= suffix->len)
               && memcmp(s->data + (s->len - suffix->len), suffix->data, (size_t)suffix->len) == 0;
}

// Horspool algorithm
isize
s8find(const s8 s PTR, const s8 sub PTR) {
        if (s->len < sub->len) {
                return -1;
        }
        if (sub->len == 0) {
                return 0;
        }
        isize last_occ[U8ALPHABET];
        memset(last_occ, -1, U8SIZE * U8ALPHABET);
        for (isize i = 0; i < sub->len - 1; ++i) {
                last_occ[sub->data[i]] = i;
        }

        for (isize i = 0; i <= s->len - sub->len;) {
                for (isize j = sub->len - 1; sub->data[j] == s->data[j + i];) {
                        if (--j == -1) {
                                return i;
                        }
                }
                i += sub->len - 1 - last_occ[s->data[i + sub->len - 1]];
        }
        return -1;
}

isize
s8count(const s8 s PTR, const s8 sub PTR) {
        if (s->len < sub->len) {
                return 0;
        }
        if (sub->len == 0) {
                return s->len;
        }
        isize count = 0;
        isize last_occ[U8ALPHABET];
        memset(last_occ, -1, U8SIZE * U8ALPHABET);
        for (isize i = 0; i < sub->len - 1; ++i) {
                last_occ[sub->data[i]] = i;
        }

        for (isize i = 0; i <= s->len - sub->len;) {
                for (isize j = sub->len - 1; sub->data[j] == s->data[j + i];) {
                        if (--j == -1) {
                                ++count;
                                break;
                        }
                }
                i += sub->len - 1 - last_occ[s->data[i + sub->len - 1]];
        }
        return count;
}
