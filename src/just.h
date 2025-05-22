// @author Egor Afanasin <afanasin.egor@gmail.com>
#ifndef JUST_H
#define JUST_H

#include <assert.h>
#include <ctype.h>
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
#define alloc(arena, type)     (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define allocn(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), n)
INLINE arena_savepoint arena_save(arena* a) { return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { save.arena->used = save.used; }

// string ------------------------------------------------------------------------------------------
typedef struct {
        char* data;
        int len;
} string;
#define PRI_string "%.*s"
#define FMT_string(string) (string).len, (string).data
typedef struct {
        string* data;
        int len;
} strings;

#define S(literal) (string){.data = (literal), .len = (int)(sizeof(literal) - 1)}
#define SA(literal, arena) string_new(arena, literal, sizeof(literal) - 1)
string string_new(arena* a, const char* data, size_t len);
string string_fmt(arena* a, const char* fmt, ...);
string string_dup(arena* a, string s);
string string_cat(arena* a, string self, string s);
string string_inject(arena* a, string self, int index, int len, string s);

int string_cmp  (string s1, string s2);
int string_icmp (string s1, string s2);
INLINE bool string_eq (string s1, string s2) { return s1.len == s2.len && memcmp(s1.data, s2.data, (size_t)s1.len) == 0; }
       bool string_ieq(string s1, string s2);
INLINE bool string_startswith(string s, string prefix) { return (s.len >= prefix.len) && memcmp(s.data, prefix.data, (size_t)prefix.len) == 0; }
INLINE bool string_endswith  (string s, string suffix) { return (s.len >= suffix.len) && memcmp(s.data + (s.len - suffix.len), suffix.data, (size_t)suffix.len) == 0; }
       bool string_contains  (string s, string sub); // in

string string_ljust  (arena* a, string s, int width, char c);
string string_rjust  (arena* a, string s, int width, char c);
string string_center (arena* a, string s, int width, char c); // or len
string string_join   (arena* a, strings ss); // const char* instead of string when needed

string  string_view  (string s, int index, int len);
int     string_find  (string s, string sub);
int     string_rfind (string s, string sub);
int     string_count (string s, string sub);
strings string_split(arena* a, string s, string sep);
// splitlines

static const string SPACE = {.data = " \t\n\r\f\v", .len = 5};
static const string DIGIT = {.data = "0123456789", .len = 10};
static const string HEX   = {.data = "0123456789ABCDEFabcdef", .len = 22};
static const string UPPER = {.data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ", .len = 26};
static const string LOWER = {.data = "abcdefghijklmnopqrstuvwxyz", .len = 26};
static const string ALPHA = {.data = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", .len = 52};
static const string WORD  = {.data = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_", .len = 63};
static const string PUNCT = {.data = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", .len = 32};
string  string_lstrip(string s, string chars);
string  string_rstrip(string s, string chars);
string  string_strip (string s, string chars);
int     string_fany  (string s, string chars); // scan search
int     string_rfany (string s, string chars); // rscan
string  string_remap (arena* a, string s, string chars, string new); // swap
strings string_cut   (arena* a, string s, string chars); // chop

INLINE bool string_isdigit (string s) { for (int i = 0; i < s.len; ++i) { if (!isdigit (s.data[i])) return false; } return true; }
INLINE bool string_isupper (string s) { for (int i = 0; i < s.len; ++i) { if (!isupper (s.data[i])) return false; } return true; }
INLINE bool string_islower (string s) { for (int i = 0; i < s.len; ++i) { if (!islower (s.data[i])) return false; } return true; }
INLINE bool string_isprint (string s) { for (int i = 0; i < s.len; ++i) { if (!isprint (s.data[i])) return false; } return true; }
INLINE bool string_isgraph (string s) { for (int i = 0; i < s.len; ++i) { if (!isgraph (s.data[i])) return false; } return true; }
INLINE bool string_isalpha (string s) { for (int i = 0; i < s.len; ++i) { if (!isalpha (s.data[i])) return false; } return true; }
INLINE bool string_isalnum (string s) { for (int i = 0; i < s.len; ++i) { if (!isalnum (s.data[i])) return false; } return true; }
INLINE bool string_isxdigit(string s) { for (int i = 0; i < s.len; ++i) { if (!isxdigit(s.data[i])) return false; } return true; }
INLINE bool string_isblank (string s) { for (int i = 0; i < s.len; ++i) { if (!isblank (s.data[i])) return false; } return true; }
INLINE bool string_isspace (string s) { for (int i = 0; i < s.len; ++i) { if (!isspace (s.data[i])) return false; } return true; }
INLINE bool string_iscntrl (string s) { for (int i = 0; i < s.len; ++i) { if (!iscntrl (s.data[i])) return false; } return true; }
INLINE bool string_ispunct (string s) { for (int i = 0; i < s.len; ++i) { if (!ispunct (s.data[i])) return false; } return true; }
       bool string_istitle (string s);
INLINE string string_upper     (string s) { for (int i = 0; i < s.len; ++i) { s.data[i] = (char)toupper(s.data[i]); } return s; }
INLINE string string_lower     (string s) { for (int i = 0; i < s.len; ++i) { s.data[i] = (char)tolower(s.data[i]); } return s; }
       string string_capitalize(string s);
       string string_title     (string s);

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
