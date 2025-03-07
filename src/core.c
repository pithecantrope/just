#include "core.h"

// arena -------------------------------------------------------------------------------------------
static arena_region*
region(arena* a, u32 capacity) {
        capacity += alignof(max_align_t);
        bool vacant = true;
        if (capacity > ARENA_REGION_CAPACITY) {
                vacant = false;
        } else {
                capacity = ARENA_REGION_CAPACITY;
                abort_if(a->vacants == ARENA_VACANT_REGIONS);
        }
        arena_region* r = malloc(sizeof(arena_region) + capacity);
        abort_if(r == NULL);
        *r = (arena_region){.cap = capacity};
        ++a->regions;
        if (vacant) {
                a->vacant[a->vacants++] = r;
        }
        return r;
}

void*
arena_alloc(arena a PTR, u32 size, u32 align, u32 count) {
        assert(IS_POW2(align));
        abort_if(size == 0 || count > (UINT32_MAX - alignof(max_align_t)) / size);
        u32 capacity = size * count;
        arena_region* r = NULL;
        if (a->regions == 0) {
                assert(a->head == NULL && a->tail == NULL && a->vacants == 0);
                r = a->head = a->tail = region(a, capacity);
        } else {
                for (u32 i = 0; i < a->vacants; ++i) {
                        r = a->vacant[i];
                        u32 l = r->cap - r->used - (-(uintptr_t)(r->data + r->used) & (align - 1));
                        if (l < capacity) {
                                continue;
                        }
                        if (l - capacity < alignof(max_align_t)) {
                                for (u32 j = i; j < a->vacants - 1; ++j) {
                                        a->vacant[j] = a->vacant[j + 1];
                                }
                                --a->vacants;
                        }
                        break;
                }
                r = (r == NULL) ? (a->tail = a->tail->next = region(a, capacity)) : r;
        }
        u32 padding = -(uintptr_t)(r->data + r->used) & (align - 1);
        void* ptr = r->data + r->used + padding;
        r->used += padding + capacity;
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

inline void
arena_reset(arena a PTR) {
        for (arena_region* r = a->head; r != NULL; r = r->next) {
                r->used = 0;
        }
}

// arena_savepoint
// arena_save(arena a PTR) {
//         return (arena_savepoint){
//                 .arena = a, .regions = a->regions};
// }

// void
// arena_restore(arena_savepoint save PTR) {
// }

// s8 ----------------------------------------------------------------------------------------------
bool
s8is_title(const s8 s PTR) {
        TODO(s);
        return true;
}

s8*
s8title(s8 s PTR) {
        return s;
}

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
        memset(last_occ, -1, sizeof(isize) * U8ALPHABET);
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
        memset(last_occ, -1, sizeof(isize) * U8ALPHABET);
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
