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
#define INLINE static inline
#define JUST2X(a, b, NAME, ...) NAME
#define JUST3X(a, b, c, NAME, ...) NAME
#define JUST4X(a, b, c, d, NAME, ...) NAME

// arena -------------------------------------------------------------------------------------------
typedef struct {
        size_t used, cap;
        char data[];
} arena;
#define PRI_arena "{used:%zu, cap:%zu, data:%p}"
#define FMT_arena(arena) (arena)->used, (arena)->cap, (arena)->data
typedef struct {
        arena* arena;
        size_t used;
} arena_savepoint;

arena* arena_create(size_t capacity);
INLINE void arena_reset  (arena* a) { a->used = 0; }
INLINE void arena_destroy(arena* a) { free(a); }
void*   arena_alloc(arena* a, size_t align, size_t size, size_t count);
#define JUST_ALLOC2(arena, type)    (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define JUST_ALLOC3(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), n)
#define alloc(...) JUST3X(__VA_ARGS__, JUST_ALLOC3, JUST_ALLOC2, 0, 0)(__VA_ARGS__)
INLINE arena_savepoint arena_save(arena* a) { return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { save.arena->used = save.used; }

// ascii -------------------------------------------------------------------------------------------
typedef u8 ascii;
#define ASCII 0x7F
#define ISASCII(c) ISIN(0, c, ASCII)
INLINE bool  ascii_isdigit (ascii c) { return ISIN('0', c, '9'); }
INLINE bool  ascii_isupper (ascii c) { return ISIN('A', c, 'Z'); }
INLINE bool  ascii_islower (ascii c) { return ISIN('a', c, 'z'); }
INLINE bool  ascii_isprint (ascii c) { return ISIN(' ', c, '~'); }
INLINE bool  ascii_isgraph (ascii c) { return ISIN(' ' + 1, c, '~'); }
INLINE bool  ascii_isalpha (ascii c) { return ascii_isupper(c) || ascii_islower(c); }
INLINE bool  ascii_isalnum (ascii c) { return ascii_isdigit(c) || ascii_isalpha(c); }
INLINE bool  ascii_isxdigit(ascii c) { return ascii_isdigit(c) || ISIN('A', c, 'F') || ISIN('a', c, 'f'); }
INLINE bool  ascii_isblank (ascii c) { return c == ' ' || c == '\t'; }
INLINE bool  ascii_isspace (ascii c) { return ascii_isblank(c) || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
INLINE bool  ascii_iscntrl (ascii c) { return c < ' ' || c == ASCII; }
INLINE bool  ascii_ispunct (ascii c) { return ascii_isgraph(c) && !ascii_isalnum(c); }
INLINE ascii ascii_upper   (ascii c) { return ascii_islower(c) ? c - ('a' - 'A') : c; }
INLINE ascii ascii_lower   (ascii c) { return ascii_isupper(c) ? c + ('a' - 'A') : c; }
INLINE ascii ascii_swapcase(ascii c) { return ascii_isupper(c) ? ascii_lower(c) : (ascii_islower(c) ? ascii_upper(c) : c); }

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

#endif // JUST_H
