#include "core.h"

// arena -------------------------------------------------------------------------------------------
static arena_region*
alloc_region(usize capacity) {
        capacity = MAX(capacity, ARENA_REGION_CAPACITY);
        abort_if(sizeof(arena_region) > USIZE_MAX - capacity);
        arena_region* r = malloc(sizeof(arena_region) + capacity);
        abort_if(r == NULL);
        *r = (arena_region){.total = capacity};
        return r;
}

static inline usize
padding(const arena_region* r, usize align) {
        return -(uintptr_t)(r->data + r->used) & (align - 1);
}

void*
arena_alloc(arena a PTR, usize size, usize align, usize count) {
        assert(IS_POW2(align));
        abort_if(size == 0 || count > USIZE_MAX / size);
        usize capacity = size * count;
        arena_region* r = a->head;
        if (r == NULL) {
                ++a->regions;
                r = a->head = alloc_region(capacity);
        } else {
                for (; r != NULL && capacity > r->total - r->used - padding(r, align);
                     r = r->next) {}
                if (r == NULL) {
                        ++a->regions;
                        r = alloc_region(capacity);
                        r->next = a->head;
                        a->head = r;
                }
        }
        usize pad = padding(r, align);
        void* ptr = r->data + r->used + pad;
        r->used += pad + capacity;
        return ptr;
}

void
arena_free(arena a PTR) {
        for (arena_region *next, *r = a->head; r != NULL; r = next) {
                next = r->next;
                free(r);
        }
        *a = (arena){0};
}

void
arena_reset(arena a PTR) {
        for (arena_region* r = a->head; r != NULL; r = r->next) {
                r->used = 0;
        }
}

arena_savepoint
arena_save(arena a PTR) {
        if (a->regions == 0) {
                assert(a->head == NULL);
                return (arena_savepoint){0};
        }
        usize* ptr = malloc(sizeof(usize) * a->regions);
        abort_if(ptr == NULL);
        usize* p = ptr;
        for (arena_region* r = a->head; r != NULL; r = r->next) {
                *(p++) = r->used;
        }
        return (arena_savepoint){.arena = a, .regions = a->regions, .ptr = ptr};
}

void
arena_restore(arena_savepoint save PTR) {
        if (save->arena == NULL) {
                assert(save->regions == 0 && save->ptr == NULL);
                return;
        }
        arena_region* r = save->arena->head;
        for (usize i = 0; i < save->arena->regions - save->regions; ++i, r = r->next) {
                r->used = 0;
        }
        usize* p = save->ptr;
        for (; r != NULL; r = r->next) {
                r->used = *(p++);
        }
        free(save->ptr);
        *save = (arena_savepoint){0};
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
