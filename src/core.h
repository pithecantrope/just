/**
 * @file      core.h
 * @brief     Core utilities
 * @author    Egor Afanasin <afanasin.egor@gmail.com>
 * @copyright GPL-3.0
 * @details
 * Types:
 * - Basic: `byte`, `i8`, `u8`, `i16`, `u16`, `i32`, `u32`, `i64`, `u64`, `f32`, `f64`, `isize`, `usize`
 * - Rich: `u8` (UTF-8 char), `arena` (linear allocator), `s8` (UTF-8 string)
 * General:
 * - `MIN(a, b)`, `MAX(a, b)`, `ABS(x)`, `DIFF(a, b)`
 * - `IS_POW2(x)`: Check if x is a power of 2
 * - `IS_IN(min, x, max)`: Check if x is within the range [min, max]
 * - `abort_if(condition)`: Abort execution if condition is true
 * - `PTR`: Non-NULL pointer
 * - `INLINE`: Alias for `static inline`
 * - `TODO(...)`: Suppress unused arguments warnings
 * - `TEST(name)`: Define a test case with the specified name
 * - `EXPECT(condition)`: Check if condition is true within a test case
 * `u8` (UTF-8 char):
 * - `is_digit`, `is_upper`, `is_lower`, `is_alpha`, `is_alnum`, `is_xdigit`
 * - `is_print`, `is_graph`, `is_blank`, `is_space`, `is_ascii`, `is_cntrl`, `is_punct`
 * - `upper`, `lower`, `swapcase`
 * `arena` (linear allocator):
 * - `arena_new(capacity)`, `arena_reset(arena)`, `arena_delete(arena)`: Allocate/renew/free the arena
 * - `arena_alloc(arena, size, align, count)`: Allocate with explicit alignment control
 * - `alloc(arena, type[, count])`: Allocate memory for `count` objects of a specified type
 * - `arena_save(arena)`, `arena_restore(save)`: Capture/recover the state of arena
 * `s8` (UTF-8 string):
 * - `is_digit`, `is_upper`, `is_lower`, `is_alpha`, `is_alnum`, `is_xdigit`, `is_title`
 * - `is_print`, `is_graph`, `is_blank`, `is_space`, `is_ascii`, `is_cntrl`, `is_punct`
 * - `upper`, `lower`, `swapcase`, `capitalize`, `title`
 * - `cmp(s1, s2)`, `eq(s1, s2)`, `starts_with(s, prefix)`, `ends_with(s, suffix)`
 * - `find(s, sub)`, `count(s, sub)`
 * - `s8lower(s)`, `s8upper(s)`, `s8title(s)`, `s8swapcase(s)`, `s8capitalize(s)`
 */

#ifndef CORE_H_
#define CORE_H_

#include <assert.h>
#include <float.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char      byte;
#define BYTE_MIN  CHAR_MIN
#define BYTE_MAX  CHAR_MAX
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

#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define ABS(x)     (((x) > 0) ? (x) : -(x))
#define DIFF(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))
#define IS_POW2(x) (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define IS_IN(min, x, max) ((min) <= (x) && (x) <= (max))
#define abort_if(condition) if (condition) abort()
#define PTR [static 1]
#define INLINE static inline
#define TODO(...) (CORE_TODO_1(__VA_ARGS__, 0))
#if 0   // Example:
// tests.h:
        #ifdef TESTS
        TEST("true") {
                EXPECT(!0);
        }
        TEST("false") {
                EXPECT(0 == !0);
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
#define TEST(name) TEST = name;
#define EXPECT(condition) if (!(condition)) printf("FAIL: %s:%d: Test `%s':\n\tCondition: `%s'\n", __FILE__, __LINE__, TEST, #condition)

// u8 ----------------------------------------------------------------------------------------------
#define U8ALPHABET 256
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
INLINE bool u8is_ascii (u8 c) { return c <= 127; }
INLINE bool u8is_cntrl (u8 c) { return c < ' ' || c == 127; }
INLINE bool u8is_punct (u8 c) { return u8is_graph(c) && !u8is_alnum(c); }
INLINE u8   u8upper    (u8 c) { return u8is_lower(c) ? c - ('a' - 'A') : c; }
INLINE u8   u8lower    (u8 c) { return u8is_upper(c) ? c + ('a' - 'A') : c; }
INLINE u8   u8swapcase (u8 c) { return u8is_upper(c) ? u8lower(c) : (u8is_lower(c) ? u8upper(c) : c); }

// arena -------------------------------------------------------------------------------------------
typedef struct {
        usize used, cap;
        byte data[];
} arena;

arena* arena_new(usize capacity);
void* arena_alloc(arena* a, usize size, usize align, usize count);
#define alloc(...) CORE_ALLOCX(__VA_ARGS__, CORE_ALLOC3, CORE_ALLOC2, 0)(__VA_ARGS__)
INLINE void arena_reset (arena* a) { a->used = 0; }
INLINE void arena_delete(arena* a) { free(a); }

typedef struct {
        arena* arena;
        usize used;
} arena_savepoint;
INLINE arena_savepoint arena_save(arena* a) { return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { save.arena->used = save.used; }

// s8 ----------------------------------------------------------------------------------------------
typedef struct {
        u8* data;
        isize len;
} s8;
#define s8(s) &(s8){.data = (u8*)(s), .len = (isize)(sizeof(s) - 1)}

// TODO: s8(a, s) should call s8cstr
s8* s8cstr(arena* a, const char data PTR, size_t len);
s8* s8copy(arena* a, const s8 s PTR);

INLINE bool s8is_digit  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_digit (s->data[i])) return false; } return true; }
INLINE bool s8is_upper  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_upper (s->data[i])) return false; } return true; }
INLINE bool s8is_lower  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_lower (s->data[i])) return false; } return true; }
INLINE bool s8is_print  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_print (s->data[i])) return false; } return true; }
INLINE bool s8is_graph  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_graph (s->data[i])) return false; } return true; }
INLINE bool s8is_alpha  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_alpha (s->data[i])) return false; } return true; }
INLINE bool s8is_alnum  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_alnum (s->data[i])) return false; } return true; }
INLINE bool s8is_xdigit (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_xdigit(s->data[i])) return false; } return true; }
INLINE bool s8is_blank  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_blank (s->data[i])) return false; } return true; }
INLINE bool s8is_space  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_space (s->data[i])) return false; } return true; }
INLINE bool s8is_ascii  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_ascii (s->data[i])) return false; } return true; }
INLINE bool s8is_cntrl  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_cntrl (s->data[i])) return false; } return true; }
INLINE bool s8is_punct  (const s8 s PTR) { for (isize i = 0; i < s->len; ++i) { if (!u8is_punct (s->data[i])) return false; } return true; }
       bool s8is_title  (const s8 s PTR);
INLINE s8*  s8upper     (s8 s PTR) { for (isize i = 0; i < s->len; ++i) { s->data[i] = u8upper   (s->data[i]); } return s; }
INLINE s8*  s8lower     (s8 s PTR) { for (isize i = 0; i < s->len; ++i) { s->data[i] = u8lower   (s->data[i]); } return s; }
INLINE s8*  s8swapcase  (s8 s PTR) { for (isize i = 0; i < s->len; ++i) { s->data[i] = u8swapcase(s->data[i]); } return s; }
       s8*  s8capitalize(s8 s PTR);
       s8*  s8title     (s8 s PTR);

int   s8cmp (const s8 s1 PTR, const s8 s2 PTR);
int   s8icmp(const s8 s1 PTR, const s8 s2 PTR);
INLINE bool s8eq (const s8 s1 PTR, const s8 s2 PTR) { return  s8cmp(s1, s2) == 0; }
INLINE bool s8ieq(const s8 s1 PTR, const s8 s2 PTR) { return s8icmp(s1, s2) == 0; }
INLINE bool s8starts_with(const s8 s PTR, const s8 prefix PTR) { return (s->len >= prefix->len) && memcmp(s->data, prefix->data, (size_t)prefix->len) == 0; }
INLINE bool s8ends_with  (const s8 s PTR, const s8 suffix PTR) { return (s->len >= suffix->len) && memcmp(s->data + (s->len - suffix->len), suffix->data, (size_t)suffix->len) == 0; }
isize s8find (const s8 s PTR, const s8 sub PTR);
isize s8rfind(const s8 s PTR, const s8 sub PTR);
isize s8count(const s8 s PTR, const s8 sub PTR);

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
#define CORE_ALLOC3(a, t, n) (t*)arena_alloc(a, sizeof(t), alignof(t), n)

#endif // CORE_H_
