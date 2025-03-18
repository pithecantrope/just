/**
 * @author    Egor Afanasin <afanasin.egor@gmail.com>
 * @copyright GPL-3.0
 */

#ifndef CORE_H
#define CORE_H

#include <assert.h>
#include <float.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
typedef float     f32;
#define F32_MIN   FLT_MIN
#define F32_MAX   FLT_MAX
typedef double    f64;
#define F64_MIN   DBL_MIN
#define F64_MAX   DBL_MAX
typedef ptrdiff_t isize;
#define ISIZE_MIN PTRDIFF_MIN
#define ISIZE_MAX PTRDIFF_MAX
typedef size_t    usize;
#define USIZE_MAX SIZE_MAX

typedef struct {
        usize used, cap;
        byte data[];
} arena;
typedef struct {
        arena* arena;
        usize used;
} arena_savepoint;

typedef struct {
        u8* data;
        isize len;
} s8;
typedef struct {
        isize* data;
        isize len;
} indexes;

// General -----------------------------------------------------------------------------------------
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define ABS(x)     (((x) > 0) ? (x) : -(x))
#define DIFF(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))
#define IS_POW2(x) (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define IS_IN(min, x, max) ((min) <= (x) && (x) <= (max))
#define CLAMP(min, x, max) ((x) < (min) ? (min) : (((x) > (max)) ? (max) : (x)))
#define countof(xs) (sizeof(xs) / sizeof(0[xs]))
#define containerof(ptr, type, member) ((type*)((byte*)(ptr) - offsetof(type, member)))
#define abort_if(condition) if (condition) abort()
#define INLINE static inline
#define TODO(...) (CORE_TODO_1(__VA_ARGS__, 0))

// Test  -------------------------------------------------------------------------------------------
#define TEST(name) TEST = name;
#define EXPECT(condition) if (!(condition)) printf("FAIL: %s:%d: Test `%s':\n\tCondition: `%s'\n", __FILE__, __LINE__, TEST, #condition)
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
        #include "core.h"
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
void*   arena_alloc(arena* a, usize size, usize align, usize count);
#define alloc(...) CORE_ALLOCX(__VA_ARGS__, CORE_ALLOC3, CORE_ALLOC2, 0)(__VA_ARGS__)
INLINE arena_savepoint arena_save(arena* a) { return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { save.arena->used = save.used; }

// s8 ----------------------------------------------------------------------------------------------
#define s8(s) (s8){.data = (u8*)(s), .len = (isize)(sizeof(s) - 1)}
#define S8(a, s) s8new(a, s, sizeof(s) - 1)
s8 s8new   (arena* a, const char* data, usize len);
s8 s8dup   (arena* a, s8 s);
s8 s8slice (arena* a, s8 s, isize start, isize stop, isize step);
s8 s8repeat(arena* a, s8 s, isize n);
s8 s8cat   (arena* a, s8 s1, s8 s2);
s8 s8inject(arena* a, s8 s1, isize index, isize len, s8 s2);

i32 s8cmp (s8 s1, s8 s2);
i32 s8icmp(s8 s1, s8 s2);
INLINE bool s8eq (s8 s1, s8 s2) { return s8cmp (s1, s2) == 0; }
INLINE bool s8ieq(s8 s1, s8 s2) { return s8icmp(s1, s2) == 0; }
INLINE bool s8starts_with(s8 s, s8 prefix) { return (s.len >= prefix.len) && memcmp(s.data, prefix.data, (usize)prefix.len) == 0; }
INLINE bool s8ends_with  (s8 s, s8 suffix) { return (s.len >= suffix.len) && memcmp(s.data + (s.len - suffix.len), suffix.data, (usize)suffix.len) == 0; }

s8 s8span(s8 s, isize index, isize len);
isize s8find (s8 s, s8 sub);
isize s8count(s8 s, s8 sub);
indexes s8findall(arena* a, s8 s, s8 sub);

INLINE bool s8is_digit  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_digit (s.data[i])) return false; } return true; }
INLINE bool s8is_upper  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_upper (s.data[i])) return false; } return true; }
INLINE bool s8is_lower  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_lower (s.data[i])) return false; } return true; }
INLINE bool s8is_print  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_print (s.data[i])) return false; } return true; }
INLINE bool s8is_graph  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_graph (s.data[i])) return false; } return true; }
INLINE bool s8is_alpha  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_alpha (s.data[i])) return false; } return true; }
INLINE bool s8is_alnum  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_alnum (s.data[i])) return false; } return true; }
INLINE bool s8is_xdigit (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_xdigit(s.data[i])) return false; } return true; }
INLINE bool s8is_blank  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_blank (s.data[i])) return false; } return true; }
INLINE bool s8is_space  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_space (s.data[i])) return false; } return true; }
INLINE bool s8is_ascii  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_ascii (s.data[i])) return false; } return true; }
INLINE bool s8is_cntrl  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_cntrl (s.data[i])) return false; } return true; }
INLINE bool s8is_punct  (s8 s) { for (isize i = 0; i < s.len; ++i) { if (!u8is_punct (s.data[i])) return false; } return true; }
       bool s8is_title  (s8 s);
// Internals ---------------------------------------------------------------------------------------
#define CORE_TODO_1(arg, ...) (void)arg, CORE_TODO_2 (__VA_ARGS__, 0)
#define CORE_TODO_2(arg, ...) (void)arg, CORE_TODO_3 (__VA_ARGS__, 0)
#define CORE_TODO_3(arg, ...) (void)arg, CORE_TODO_4 (__VA_ARGS__, 0)
#define CORE_TODO_4(arg, ...) (void)arg, CORE_TODO_5 (__VA_ARGS__, 0)
#define CORE_TODO_5(arg, ...) (void)arg, CORE_TODO_6 (__VA_ARGS__, 0)
#define CORE_TODO_6(arg, ...) (void)arg, CORE_TODO_7 (__VA_ARGS__, 0)
#define CORE_TODO_7(arg, ...) (void)arg, CORE_TODO_8 (__VA_ARGS__, 0)
#define CORE_TODO_8(arg, ...) (void)arg

#define CORE_ALLOCX(_1, _2, _3, NAME, ...) NAME
#define CORE_ALLOC2(a, t)    (t*)arena_alloc(a, sizeof(t), alignof(t), 1)
#define CORE_ALLOC3(a, t, n) (t*)arena_alloc(a, sizeof(t), alignof(t), (usize)(n))

#endif // CORE_H
