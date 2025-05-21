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
