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
#include <stdio.h>  // printf/putchar/fputs
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

typedef struct {
        u8* data;
        isize len;
} s8;

// General -----------------------------------------------------------------------------------------
#define INLINE             static inline
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define DIFF(a, b)         ((a) > (b) ? (a) - (b) : (b) - (a))
#define ABS(x)             (((x) > 0) ? (x) : -(x))
#define IS_POW2(x)         (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define IS_IN(min, x, max) ((min) <= (x) && (x) <= (max))
#define TODO(...)          JUST8X(__VA_ARGS__, JUST_TODO8, JUST_TODO7, JUST_TODO6, JUST_TODO5, JUST_TODO4, JUST_TODO3, JUST_TODO2, JUST_TODO1, 0)(__VA_ARGS__)
static char* PRINT_SEP = " ";
#define print(...)         do { JUST16X(__VA_ARGS__, JUST_PRINT16, JUST_PRINT15, JUST_PRINT14, JUST_PRINT13, JUST_PRINT12, JUST_PRINT11, JUST_PRINT10, JUST_PRINT9, JUST_PRINT8, JUST_PRINT7, JUST_PRINT6, JUST_PRINT5, JUST_PRINT4, JUST_PRINT3, JUST_PRINT2, JUST_PRINT1, 0)(__VA_ARGS__); } while(0)
#define println(...)       do { JUST16X(__VA_ARGS__, JUST_PRINT16, JUST_PRINT15, JUST_PRINT14, JUST_PRINT13, JUST_PRINT12, JUST_PRINT11, JUST_PRINT10, JUST_PRINT9, JUST_PRINT8, JUST_PRINT7, JUST_PRINT6, JUST_PRINT5, JUST_PRINT4, JUST_PRINT3, JUST_PRINT2, JUST_PRINT1, 0)(__VA_ARGS__); putchar('\n'); } while(0)

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

// Pseudo-random -----------------------------------------------------------------------------------
f64 rng01(void);

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
#define JUST_ALLOC2(a, t)    (t*)arena_alloc(a, alignof(t), sizeof(t), 1)
#define JUST_ALLOC3(a, t, n) (t*)arena_alloc(a, alignof(t), sizeof(t), (usize)(n))
#define alloc(...) JUST3X(__VA_ARGS__, JUST_ALLOC3, JUST_ALLOC2, 0, 0)(__VA_ARGS__)
INLINE arena_savepoint arena_save(arena* a) { return (arena_savepoint){.arena = a, .used = a->used}; }
INLINE void arena_restore(arena_savepoint save) { save.arena->used = save.used; }

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

#define JUST_PRINT(x) _Generic((x),                                                                \
        char:               putchar,                                                               \
        signed char:        JUST_PRINT_schar,                                                      \
        unsigned char:      JUST_PRINT_uchar,                                                      \
        short:              JUST_PRINT_short,                                                      \
        unsigned short:     JUST_PRINT_ushort,                                                     \
        int:                JUST_PRINT_int,                                                        \
        unsigned:           JUST_PRINT_uint,                                                       \
        long:               JUST_PRINT_long,                                                       \
        unsigned long:      JUST_PRINT_ulong,                                                      \
        long long:          JUST_PRINT_llong,                                                      \
        unsigned long long: JUST_PRINT_ullong,                                                     \
        float:              JUST_PRINT_double,                                                     \
        double:             JUST_PRINT_double,                                                     \
        long double:        JUST_PRINT_ldouble,                                                    \
        void*:              JUST_PRINT_ptr,                                                        \
        const void*:        JUST_PRINT_ptr,                                                        \
        char*:              JUST_PRINT_str,                                                        \
        const char*:        JUST_PRINT_str,                                                        \
        bool:               JUST_PRINT_bool,                                                       \
        arena*:             JUST_PRINT_arena,                                                      \
        arena_savepoint:    JUST_PRINT_arena_savepoint,                                            \
        s8:                 JUST_PRINT_s8                                                          \
)(x)
INLINE void JUST_PRINT_schar  (signed char        x) { printf("%hhd", x); }
INLINE void JUST_PRINT_uchar  (unsigned char      x) { printf("%hhu", x); }
INLINE void JUST_PRINT_short  (short              x) { printf("%hd",  x); }
INLINE void JUST_PRINT_ushort (unsigned short     x) { printf("%hu",  x); }
INLINE void JUST_PRINT_int    (int                x) { printf("%d",   x); }
INLINE void JUST_PRINT_uint   (unsigned           x) { printf("%u",   x); }
INLINE void JUST_PRINT_long   (long               x) { printf("%ld",  x); }
INLINE void JUST_PRINT_ulong  (unsigned long      x) { printf("%lu",  x); }
INLINE void JUST_PRINT_llong  (long long          x) { printf("%lld", x); }
INLINE void JUST_PRINT_ullong (unsigned long long x) { printf("%llu", x); }
INLINE void JUST_PRINT_double (double             x) { printf("%lf",  x); }
INLINE void JUST_PRINT_ldouble(long double        x) { printf("%Lf",  x); }
INLINE void JUST_PRINT_ptr    (const void*        x) { printf("%p",   x); }
INLINE void JUST_PRINT_str    (const char*        x) { fputs(x, stdout); }
INLINE void JUST_PRINT_bool   (bool               x) { fputs(x ? "true" : "false", stdout); }
INLINE void JUST_PRINT_arena  (arena*             x) { printf("{used:%zu, cap:%zu, data:%p}", x->used, x->cap, (void*)x->data); }
INLINE void JUST_PRINT_arena_savepoint(arena_savepoint x) { printf("{arena:%p, used:%zu}", (void*)x.arena, x.used); }
INLINE void JUST_PRINT_s8     (s8                 x) { for(isize i = 0; i < x.len; ++i) putchar(x.data[i]); }
INLINE void JUST_PRINT_sep    (void                ) { fputs(PRINT_SEP, stdout); }
#define JUST_PRINT1(a) JUST_PRINT(a)
#define JUST_PRINT2(a, b) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b)
#define JUST_PRINT3(a, b, c) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c)
#define JUST_PRINT4(a, b, c, d) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d)
#define JUST_PRINT5(a, b, c, d, e) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e)
#define JUST_PRINT6(a, b, c, d, e, f) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f)
#define JUST_PRINT7(a, b, c, d, e, f, g) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g)
#define JUST_PRINT8(a, b, c, d, e, f, g, h) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h)
#define JUST_PRINT9(a, b, c, d, e, f, g, h, i) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i)
#define JUST_PRINT10(a, b, c, d, e, f, g, h, i, j) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i); JUST_PRINT_sep(); JUST_PRINT(j)
#define JUST_PRINT11(a, b, c, d, e, f, g, h, i, j, k) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i); JUST_PRINT_sep(); JUST_PRINT(j); JUST_PRINT_sep(); JUST_PRINT(k)
#define JUST_PRINT12(a, b, c, d, e, f, g, h, i, j, k, l) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i); JUST_PRINT_sep(); JUST_PRINT(j); JUST_PRINT_sep(); JUST_PRINT(k); JUST_PRINT_sep(); JUST_PRINT(l)
#define JUST_PRINT13(a, b, c, d, e, f, g, h, i, j, k, l, m) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i); JUST_PRINT_sep(); JUST_PRINT(j); JUST_PRINT_sep(); JUST_PRINT(k); JUST_PRINT_sep(); JUST_PRINT(l); JUST_PRINT_sep(); JUST_PRINT(m)
#define JUST_PRINT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i); JUST_PRINT_sep(); JUST_PRINT(j); JUST_PRINT_sep(); JUST_PRINT(k); JUST_PRINT_sep(); JUST_PRINT(l); JUST_PRINT_sep(); JUST_PRINT(m); JUST_PRINT_sep(); JUST_PRINT(n)
#define JUST_PRINT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i); JUST_PRINT_sep(); JUST_PRINT(j); JUST_PRINT_sep(); JUST_PRINT(k); JUST_PRINT_sep(); JUST_PRINT(l); JUST_PRINT_sep(); JUST_PRINT(m); JUST_PRINT_sep(); JUST_PRINT(n); JUST_PRINT_sep(); JUST_PRINT(o)
#define JUST_PRINT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) JUST_PRINT(a); JUST_PRINT_sep(); JUST_PRINT(b); JUST_PRINT_sep(); JUST_PRINT(c); JUST_PRINT_sep(); JUST_PRINT(d); JUST_PRINT_sep(); JUST_PRINT(e); JUST_PRINT_sep(); JUST_PRINT(f); JUST_PRINT_sep(); JUST_PRINT(g); JUST_PRINT_sep(); JUST_PRINT(h); JUST_PRINT_sep(); JUST_PRINT(i); JUST_PRINT_sep(); JUST_PRINT(j); JUST_PRINT_sep(); JUST_PRINT(k); JUST_PRINT_sep(); JUST_PRINT(l); JUST_PRINT_sep(); JUST_PRINT(m); JUST_PRINT_sep(); JUST_PRINT(n); JUST_PRINT_sep(); JUST_PRINT(o); JUST_PRINT_sep(); JUST_PRINT(p)

#endif // JUST_H
