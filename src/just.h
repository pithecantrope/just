// @author Egor Afanasin <afanasin.egor@gmail.com>

// clang-format off
#ifndef JUST_H
#define JUST_H

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b)       ((a) > (b) ? (b) : (a))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define DIFF(a, b)      ((a) > (b) ? (a) - (b) : (b) - (a))
#define ABS(x)          (((x) > 0) ? (x) : -(x))
#define ISPOW2(x)       (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define ISIN(lo, x, hi) ((lo) <= (x) && (x) <= (hi))
#define INLINE static inline

// arena -------------------------------------------------------------------------------------------
typedef struct {
        size_t used, cap;
        char* data;
} arena;
#define PRI_arena "{used:%zu, cap:%zu, data:%p}"
#define FMT_arena(arena) (arena)->used, (arena)->cap, (arena)->data
typedef struct {
        arena* arena;
        size_t used;
} arena_savepoint;

arena* arena_create(size_t capacity);
INLINE void arena_reset  (arena* a) { assert(a != NULL); a->used = 0; }
INLINE void arena_destroy(arena* a) { assert(a != NULL); free(a->data); free(a); }
void*   arena_alloc(arena* a, size_t align, size_t size, size_t count);
#define alloc(arena, type)     (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define allocn(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), (size_t)(n))
INLINE arena_savepoint arena_save(arena* a) { assert(a != NULL); return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { assert(save.arena != NULL); save.arena->used = save.used; }

// string ------------------------------------------------------------------------------------------
typedef struct {
        const char* data;
        int len;
} string;
#define PRI_string "%.*s"
#define FMT_string(string) (string).len, (string).data

#define S(literal) (string){.data = (literal), .len = (int)(sizeof(literal) - 1)}
string string_new(arena* a, const char* str, size_t len);
char*  string_str(arena* a, string s);
string string_dup(arena* a, string s);
string string_cat(arena* a, string base, string s);
string string_fill(arena* a, string s, int len);
string string_file(arena* a, const char* path);
string string_sub(string s, int index, int len);

#endif // JUST_H
