// @author Egor Afanasin <afanasin.egor@gmail.com>

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

// arena -------------------------------------------------------------------------------------------
typedef struct {
        size_t used, cap;
        char* data;
} arena;

#define PRIA        "{used:%zu, cap:%zu, data:%p}"
#define FMTA(arena) (arena)->used, (arena)->cap, (arena)->data

arena* arena_create(size_t capacity);
void arena_destroy(arena* a);
void* arena_alloc(arena* a, size_t align, size_t size, size_t count);
#define alloc(arena, type)     (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define allocn(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), (size_t)(n))

typedef struct {
        arena* arena;
        size_t used;
} arena_savepoint;
arena_savepoint arena_save(arena* a);
void arena_restore(arena_savepoint save);

// string ------------------------------------------------------------------------------------------
#endif // JUST_H
