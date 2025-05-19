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

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
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
        u64 used, cap;
        byte data[];
} arena;
#define PRI_arena "{used:%" PRIu64 ", cap:%" PRIu64 ", data:%p}"
#define FMT_arena(arena) (arena)->used, (arena)->cap, (void*)(arena)->data
typedef struct {
        arena* arena;
        u64 used;
} arena_savepoint;

arena* arena_create(u64 capacity);
INLINE void arena_reset  (arena* a) { a->used = 0; }
INLINE void arena_destroy(arena* a) { free(a); }
void*   arena_alloc(arena* a, u64 align, u64 size, u64 count);
#define JUST_ALLOC2(arena, type)    (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define JUST_ALLOC3(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), (u64)(n))
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

// string ------------------------------------------------------------------------------------------
typedef struct {
        ascii* data;
        i32 len;
} string;
#define PRI_string "%.*s"
#define FMT_string(string) (int)(string).len, (char*)(string).data

#define JUST_S1(literal) (string){.data = (ascii*)(literal), .len = (i32)(sizeof(literal) - 1)}
#define JUST_S2(arena, literal) string_new(arena, literal, sizeof(literal) - 1)
#define S(...) JUST2X(__VA_ARGS__, JUST_S2, JUST_S1, 0)(__VA_ARGS__)
string string_new(arena* a, const char* data, size_t len);
string string_fmt(arena* a, const char* fmt, ...);
string string_dup(arena* a, string s);
string string_cat(arena* a, string self, string s);
string string_inject(arena* a, string self, i32 index, i32 len, string s);

int string_cmp  (string s1, string s2);
int string_icmp (string s1, string s2);
INLINE bool string_eq (string s1, string s2) { return s1.len == s2.len && memcmp(s1.data, s2.data, (size_t)s1.len) == 0; }
       bool string_ieq(string s1, string s2);
INLINE bool string_startswith(string s, string prefix) { return (s.len >= prefix.len) && memcmp(s.data, prefix.data, (size_t)prefix.len) == 0; }
INLINE bool string_endswith  (string s, string suffix) { return (s.len >= suffix.len) && memcmp(s.data + (s.len - suffix.len), suffix.data, (size_t)suffix.len) == 0; }

typedef struct {
        string* data;
        i32 len;
} strings;
string string_view(string s, i32 index, i32 len);

static const string SPACE = {(ascii*)" \t\n\r\f\v", 5};
static const string DIGIT = {(ascii*)"0123456789", 10};
static const string HEX   = {(ascii*)"0123456789ABCDEFabcdef", 22};
static const string UPPER = {(ascii*)"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26};
static const string LOWER = {(ascii*)"abcdefghijklmnopqrstuvwxyz", 26};
static const string ALPHA = {(ascii*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52};
static const string WORD  = {(ascii*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_", 63};
static const string PUNCT = {(ascii*)"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", 32};
string string_trim (string s, string chars);
string string_erase (arena* a, string s, string chars);
strings string_splitany(arena* a, string s, string chars);
string string_replaceany(arena* a, string s, string chars);

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
