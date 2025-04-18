// @author Egor Afanasin <afanasin.egor@gmail.com>
#ifndef JUST_H
#define JUST_H

// General -----------------------------------------------------------------------------------------
#include <assert.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char     byte;
typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#define INLINE             static inline
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define DIFF(a, b)         ((a) > (b) ? (a) - (b) : (b) - (a))
#define ABS(x)             (((x) > 0) ? (x) : -(x))
#define IS_POW2(x)         (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define IS_IN(min, x, max) ((min) <= (x) && (x) <= (max))

// arena -------------------------------------------------------------------------------------------
typedef struct {
        u64 used, cap;
        byte data[];
} arena;
#define FMT_arena(arena) (arena)->used, (arena)->cap, (void*)(arena)->data
#define PRI_arena "{used:%" PRIu64 ", cap:%" PRIu64 ", data:%p}"
typedef struct {
        arena* arena;
        u64 used;
} arena_savepoint;

arena* arena_create(u64 capacity);
INLINE void arena_reset  (arena* a) { a->used = 0; }
INLINE void arena_destroy(arena* a) { free(a); }
void*   arena_alloc(arena* a, u64 align, u64 size, u64 count);
#define JUST_ALLOC2(a, t)    (t*)arena_alloc(a, alignof(t), sizeof(t), 1)
#define JUST_ALLOC3(a, t, n) (t*)arena_alloc(a, alignof(t), sizeof(t), n)
#define alloc(...) JUST3X(__VA_ARGS__, JUST_ALLOC3, JUST_ALLOC2, 0, 0)(__VA_ARGS__)
INLINE arena_savepoint arena_save(arena* a) { return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { save.arena->used = save.used; }

// Testing  ----------------------------------------------------------------------------------------
#define TEST(name) TEST = name;
#define EXPECT(condition) if (!(condition)) printf("FAIL: %s:%d: Test '%s':\n\tCondition: '%s'\n", __FILE__, __LINE__, TEST, #condition)
#if 0   // Example:
// tests.h:
        #ifdef TESTS
        bool t = true;
        TEST("true") {
                EXPECT(t);
        }
        TEST("false") {
                EXPECT(!t);
        }
        #else
        // IWYU pragma: begin_keep
        #include "just.h"
        // IWYU pragma: end_keep
        #endif
// main.c:
        #include "tests.h"
        int main(void) {
                const char* TEST;
        #define TESTS
        #include "tests.h"
        #undef TESTS
                return EXIT_SUCCESS;
        }
#endif

// Internals ---------------------------------------------------------------------------------------
#define JUST2X(a, b, NAME, ...) NAME
#define JUST3X(a, b, c, NAME, ...) NAME
#define JUST4X(a, b, c, d, NAME, ...) NAME

#endif // JUST_H
