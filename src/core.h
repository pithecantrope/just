/**
 * @file      core.h
 * @brief     Core utilities
 * @author    Egor Afanasin <afanasin.egor@gmail.com>
 * @copyright GPL-3.0
 * @attention GCC/Clang only
 * @details
 * Types:
 * - Basic: `byte`, `i8`, `u8`, `i16`, `u16`, `i32`, `u32`, `i64`, `u64`, `f32`, `f64`, `isize`, `usize`
 * - Rich: `u8` (UTF-8 char), `arena` (linear allocator), `s8` (UTF-8 string)
 * General:
 * - `PTR`: Non-NULL pointer
 * - `INLINE`: `static inline`
 * - `ASSERT(c)`: Debugger-oriented assertion that in release builds turns into an optimization hint
 * - `LABEL(name)`: Inline assembly label for debugging (break or dprintf on label name)
 * - `TODO(...)`: Suppress unused arguments warnings
 * - `TEST(name)`: Define a test case
 * - `EXPECT(condition)`: Check if condition is true within a test case 
 * `u8` (UTF-8 char):
 * - is_digit, is_upper, is_lower, is_alpha, is_alnum, is_xdigit
 * - is_print, is_graph, is_blank, is_space, is_ascii, is_cntrl, is_punct
 * - to_upper, to_lower, to_ascii
 * `arena` (linear allocator):
 * -
 * `s8` (UTF-8 string):
 * - cmp(s1, s2), eq(s1, s2)
 * - starts_with(s, prefix), ends_with(s, suffix)
 * - find(s, sub), count(s, sub)
 */

#ifndef CORE_H_
#define CORE_H_

#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char        byte;
typedef int8_t      i8;
typedef uint8_t     u8;
typedef int16_t     i16;
typedef uint16_t    u16;
typedef int32_t     i32;
typedef uint32_t    u32;
typedef int64_t     i64;
typedef uint64_t    u64;
typedef float       f32;
#define F32_MIN     FLT_MIN
#define F32_MAX     FLT_MAX
typedef double      f64;
#define F64_MIN     DBL_MIN
#define F64_MAX     DBL_MAX
typedef ptrdiff_t   isize;
#define ISIZE_MIN   PTRDIFF_MIN
#define ISIZE_MAX   PTRDIFF_MAX
typedef size_t      usize;
#define USIZE_MAX   SIZE_MAX

#define PTR [static 1]
#define INLINE static inline
#define ASSERT(c)   while (!(c)) __builtin_unreachable()
#define LABEL(name) __asm__ volatile (#name ":\n\tnop")
#define TODO(...)   (CORE_TODO_1(__VA_ARGS__, 0))
#if 0
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
#define U8SIZE 8
#define U8ALPHABET 256
INLINE bool u8is_digit (u8 c) { return '0' <= c && c <= '9'; }
INLINE bool u8is_upper (u8 c) { return 'A' <= c && c <= 'Z'; }
INLINE bool u8is_lower (u8 c) { return 'a' <= c && c <= 'z'; }
INLINE bool u8is_print (u8 c) { return ' ' <= c && c <= '~'; }
INLINE bool u8is_graph (u8 c) { return ' ' + 1 <= c && c <= '~'; }
INLINE bool u8is_alpha (u8 c) { return u8is_upper(c) || u8is_lower(c); }
INLINE bool u8is_alnum (u8 c) { return u8is_digit(c) || u8is_alpha(c); }
INLINE bool u8is_xdigit(u8 c) { return u8is_digit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f'); }
INLINE bool u8is_blank (u8 c) { return c == ' ' || c == '\t'; }
INLINE bool u8is_space (u8 c) { return u8is_blank(c) || c == '\r' || c == '\n' || (c == '\f' || c == '\v'); }
INLINE bool u8is_ascii (u8 c) { return c <= 127; }
INLINE bool u8is_cntrl (u8 c) { return c < ' ' || c == 127; }
INLINE bool u8is_punct (u8 c) { return u8is_graph(c) && !u8is_alnum(c); }
INLINE u8   u8to_upper (u8 c) { return u8is_lower(c) ? c - ('a' - 'A') : c; }
INLINE u8   u8to_lower (u8 c) { return u8is_upper(c) ? c + ('a' - 'A') : c; }
INLINE u8   u8to_ascii (u8 c) { return c & 127; }

// arena -------------------------------------------------------------------------------------------

// s8 ----------------------------------------------------------------------------------------------
typedef struct {
        u8* data;
        isize len;
} s8;
#define s8(s) &(s8){.data = (u8*)s, .len = (isize)(sizeof(s) - 1)}

i32   s8cmp(const s8 s1 PTR, const s8 s2 PTR);
bool  s8eq(const s8 s1 PTR, const s8 s2 PTR);
bool  s8starts_with(const s8 s PTR, const s8 prefix PTR);
bool  s8ends_with(const s8 s PTR, const s8 suffix PTR);
isize s8find(const s8 s PTR, const s8 sub PTR);
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

#endif // CORE_H_
