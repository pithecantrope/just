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

#define INLINE          static inline
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define DIFF(a, b)      ((a) > (b) ? (a) - (b) : (b) - (a))
#define ABS(x)          (((x) > 0) ? (x) : -(x))
#define ISPOW2(x)       (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define ISIN(lo, x, hi) ((lo) <= (x) && (x) <= (hi))

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

// ascii -------------------------------------------------------------------------------------------
typedef u8 ascii;
#define ASCII 128
#define isascii(c) ISIN(0, c, ASCII - 1)
INLINE bool  ascii_isdigit (ascii c) { return ISIN('0', c, '9'); }
INLINE bool  ascii_isupper (ascii c) { return ISIN('A', c, 'Z'); }
INLINE bool  ascii_islower (ascii c) { return ISIN('a', c, 'z'); }
INLINE bool  ascii_isprint (ascii c) { return ISIN(' ', c, '~'); }
INLINE bool  ascii_isgraph (ascii c) { return ISIN(' ' + 1, c, '~'); }
INLINE bool  ascii_isalpha (ascii c) { return ascii_isupper(c) || ascii_islower(c); }
INLINE bool  ascii_isalnum (ascii c) { return ascii_isdigit(c) || ascii_isalpha(c); }
INLINE bool  ascii_isxdigit(ascii c) { return ascii_isdigit(c) || ISIN('A', c, 'F') || ISIN('a', c, 'f'); }
INLINE bool  ascii_isblank (ascii c) { return c == ' ' || c == '\t'; }
INLINE bool  ascii_isspace (ascii c) { return ascii_isblank(c) || c == '\r' || c == '\n' || c == '\f' || c == '\v'; }
INLINE bool  ascii_iscntrl (ascii c) { return c < ' ' || c == ASCII - 1; }
INLINE bool  ascii_ispunct (ascii c) { return ascii_isgraph(c) && !ascii_isalnum(c); }
INLINE ascii ascii_upper   (ascii c) { return ascii_islower(c) ? c - ('a' - 'A') : c; }
INLINE ascii ascii_lower   (ascii c) { return ascii_isupper(c) ? c + ('a' - 'A') : c; }
INLINE ascii ascii_swapcase(ascii c) { return ascii_isupper(c) ? ascii_lower(c) : (ascii_islower(c) ? ascii_upper(c) : c); }

// string ------------------------------------------------------------------------------------------
typedef struct {
        ascii* data;
        i32 len;
} string;
#define FMT_string(string) (int)(string).len, (char*)(string).data
#define PRI_string "%.*s"
#define JUST_S1(s) (string){.data = (ascii*)(s), .len = (i32)(sizeof(s) - 1)}
#define JUST_S2(a, s) string_new(a, s, sizeof(s) - 1)
#define S(...) JUST2X(__VA_ARGS__, JUST_S2, JUST_S1, 0)(__VA_ARGS__)

string string_new(arena* a, const char* data, size_t len);
string string_dup(arena* a, string s);
string string_cat(arena* a, string head, string tail);
string string_inject(arena* a, string base, i32 index, i32 len, string inject);

int string_cmp (string s1, string s2);
int string_icmp(string s1, string s2);
INLINE bool string_eq (string s1, string s2) { return string_cmp (s1, s2) == 0; }
INLINE bool string_ieq(string s1, string s2) { return string_icmp(s1, s2) == 0; }
INLINE bool string_startswith(string s, string prefix) { return (s.len >= prefix.len) && memcmp(s.data, prefix.data, (size_t)prefix.len) == 0; }
INLINE bool string_endswith  (string s, string suffix) { return (s.len >= suffix.len) && memcmp(s.data + (s.len - suffix.len), suffix.data, (size_t)suffix.len) == 0; }

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
