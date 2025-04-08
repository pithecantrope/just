/**
 * @author Egor Afanasin <afanasin.egor@gmail.com>
 */

#ifndef JUST_H
#define JUST_H

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>  // printf
#include <stdlib.h> // malloc/free
#include <string.h> // memXXX

// Types -------------------------------------------------------------------------------------------
typedef char      byte;
typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t   i16;
typedef uint16_t  u16;
typedef int32_t   i32;
typedef uint32_t  u32;
typedef int64_t   i64;
typedef uint64_t  u64;
typedef ptrdiff_t isize;
#define ISIZE_MIN PTRDIFF_MIN
#define ISIZE_MAX PTRDIFF_MAX
typedef size_t    usize;
typedef float     f32;
typedef double    f64;

typedef struct {
        usize used, cap;
        byte data[];
} arena;
typedef struct {
        arena* arena;
        usize used;
} arena_savepoint;

// General -----------------------------------------------------------------------------------------
#define INLINE             static inline
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define DIFF(a, b)         ((a) > (b) ? (a) - (b) : (b) - (a))
#define ABS(x)             (((x) > 0) ? (x) : -(x))
#define IS_POW2(x)         (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define IS_IN(min, x, max) ((min) <= (x) && (x) <= (max))
// Suppress up to 8 arguments
#define TODO(...)          (JUST_TODO_1(__VA_ARGS__, 0))

// Test  -------------------------------------------------------------------------------------------
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

// u8 ----------------------------------------------------------------------------------------------
#define U8ASCII 127
INLINE bool u8is_digit (u8 c) { return IS_IN('0', c, '9'); }
INLINE bool u8is_upper (u8 c) { return IS_IN('A', c, 'Z'); }
INLINE bool u8is_lower (u8 c) { return IS_IN('a', c, 'z'); }
INLINE bool u8is_print (u8 c) { return IS_IN(' ', c, '~'); }
INLINE bool u8is_graph (u8 c) { return IS_IN(' ' + 1, c, '~'); }
INLINE bool u8is_alpha (u8 c) { return u8is_upper(c) || u8is_lower(c); }
INLINE bool u8is_alnum (u8 c) { return u8is_digit(c) || u8is_alpha(c); }
INLINE bool u8is_xdigit(u8 c) { return u8is_digit(c) || IS_IN('A', c, 'F') || IS_IN('a', c, 'f'); }
INLINE bool u8is_blank (u8 c) { return c == ' ' || c == '\t'; }
INLINE bool u8is_space (u8 c) { return u8is_blank(c) || c == '\r' || c == '\n' || c == '\f' || c == '\v'; }
INLINE bool u8is_ascii (u8 c) { return c <= U8ASCII; }
INLINE bool u8is_cntrl (u8 c) { return c < ' ' || c == U8ASCII; }
INLINE bool u8is_punct (u8 c) { return u8is_graph(c) && !u8is_alnum(c); }
INLINE u8   u8upper    (u8 c) { return u8is_lower(c) ? c - ('a' - 'A') : c; }
INLINE u8   u8lower    (u8 c) { return u8is_upper(c) ? c + ('a' - 'A') : c; }
INLINE u8   u8swapcase (u8 c) { return u8is_upper(c) ? u8lower(c) : (u8is_lower(c) ? u8upper(c) : c); }

// arena -------------------------------------------------------------------------------------------
arena* arena_create(usize capacity);
INLINE void arena_reset  (arena* a) { a->used = 0; }
INLINE void arena_destroy(arena* a) { free(a); }
void*   arena_alloc(arena* a, usize align, usize size, usize count);
#define alloc(...) JUST_ALLOCX(__VA_ARGS__, JUST_ALLOC3, JUST_ALLOC2, 0)(__VA_ARGS__)
INLINE arena_savepoint arena_save(arena* a) { return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { save.arena->used = save.used; }

// Internals ---------------------------------------------------------------------------------------
#define JUST_TODO_1(arg, ...) (void)arg, JUST_TODO_2(__VA_ARGS__, 0)
#define JUST_TODO_2(arg, ...) (void)arg, JUST_TODO_3(__VA_ARGS__, 0)
#define JUST_TODO_3(arg, ...) (void)arg, JUST_TODO_4(__VA_ARGS__, 0)
#define JUST_TODO_4(arg, ...) (void)arg, JUST_TODO_5(__VA_ARGS__, 0)
#define JUST_TODO_5(arg, ...) (void)arg, JUST_TODO_6(__VA_ARGS__, 0)
#define JUST_TODO_6(arg, ...) (void)arg, JUST_TODO_7(__VA_ARGS__, 0)
#define JUST_TODO_7(arg, ...) (void)arg, JUST_TODO_8(__VA_ARGS__, 0)
#define JUST_TODO_8(arg, ...) (void)arg

#define JUST_ALLOCX(_1, _2, _3, NAME, ...) NAME
#define JUST_ALLOC2(a, t)                  (t*)arena_alloc(a, alignof(t), sizeof(t), 1)
#define JUST_ALLOC3(a, t, n)               (t*)arena_alloc(a, alignof(t), sizeof(t), (usize)(n))

#endif // JUST_H
