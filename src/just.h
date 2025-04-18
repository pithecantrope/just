/**
 * @author Egor Afanasin <afanasin.egor@gmail.com>
 */

#ifndef JUST_H
#define JUST_H

// General -----------------------------------------------------------------------------------------
#include <assert.h>
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
#define TODO(...)          JUST8X(__VA_ARGS__, JUST_TODO8, JUST_TODO7, JUST_TODO6, JUST_TODO5, JUST_TODO4, JUST_TODO3, JUST_TODO2, JUST_TODO1, 0)(__VA_ARGS__)

// arena -------------------------------------------------------------------------------------------
typedef struct {
        u64 used, cap;
        byte data[];
} arena;
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
        int
        main(int argc, char* argv[]) {
                TODO(argc, argv);
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
#define JUST8X(a, b, c, d, e, f, g, h, NAME, ...) NAME
#define JUST16X(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, NAME, ...) NAME

#define JUST_TODO1(a) ((void)(a))
#define JUST_TODO2(a, b) ((void)(a), (void)(b))
#define JUST_TODO3(a, b, c) ((void)(a), (void)(b), (void)(c))
#define JUST_TODO4(a, b, c, d) ((void)(a), (void)(b), (void)(c), (void)(d))
#define JUST_TODO5(a, b, c, d, e) ((void)(a), (void)(b), (void)(c), (void)(d), (void)(e))
#define JUST_TODO6(a, b, c, d, e, f) ((void)(a), (void)(b), (void)(c), (void)(d), (void)(e), (void)(f))
#define JUST_TODO7(a, b, c, d, e, f, g) ((void)(a), (void)(b), (void)(c), (void)(d), (void)(e), (void)(f), (void)(g))
#define JUST_TODO8(a, b, c, d, e, f, g, h) ((void)(a), (void)(b), (void)(c), (void)(d), (void)(e), (void)(f), (void)(g), (void)(h))

#endif // JUST_H
