/**
 * @file just.h
 * @brief String, Vector and Hashmap backed by Arena
 *
 * Arena is a linear allocator bound to an object lifetime. It groups
 * related allocations into a single memory region, enabling bulk deallocation.
 * 
 * Containers (String, Vector, Hashmap) do not own their data — the arena does.
 * This eliminates manual memory management and makes leaks impossible.
 *
 * Since the arena must be explicitly provided to each allocation function,
 * callers are forced to consider object lifetime from the start — making it
 * trivial to track where allocations occur and who owns them.
 *
 * Because allocations are simple pointer bumps inside a lifetime-bound
 * memory region, it is significantly faster than traditional malloc/free.
 *
 * @note Uses `assert()` for error handling. No return values need checking.
 *
 * @author Egor Afanasin <afanasin.egor@gmail.com>
 */

#ifndef JUST_H
#define JUST_H

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>

#define MIN(a, b)       ((a) > (b) ? (b) : (a))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define DIFF(a, b)      ((a) > (b) ? (a) - (b) : (b) - (a))
#define ABS(x)          (((x) > 0) ? (x) : -(x))
#define ISPOW2(x)       (((x) > 0) && (((x) & ((x) - 1)) == 0))
#define ISIN(lo, x, hi) ((lo) <= (x) && (x) <= (hi))

// Arena -------------------------------------------------------------------------------------------
/**
 * This arena has a fixed capacity and does not grow — always allocate 
 * enough memory upfront. Note that allocations cannot be freed
 * individually — to reclaim memory, reset or destroy the entire arena.
 */

/// A linear allocator tied to a specific lifetime
typedef struct {
        size_t used, cap;
        char* data;
} arena;

// printf(PRIA"\n", FMTA(arena));
#define PRIA        "{used:%zu, cap:%zu, data:%p}"
#define FMTA(arena) (arena)->used, (arena)->cap, (void*)(arena)->data

arena* arena_create(size_t capacity);
void arena_reset(arena* a);
void arena_destroy(arena* a);

/// Prefer `alloc()` and `allocn()` macros instead
void* arena_alloc(arena* a, size_t align, size_t size, size_t count);
#define alloc(arena, type)     (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define allocn(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), (size_t)(n))

// String ------------------------------------------------------------------------------------------
/**
 * Functions with `mut_` prefix in parameters can mutate the contents.
 * Use `string_dup()` to create an owned `string` copy explicitly. 
 */

/// A non-owning, mutable view
typedef struct {
        char* data;
        int len;
} string;

// printf(PRIS"\n", FMTS(string));
#define PRIS         "%.*s"
#define FMTS(string) (string).len, (string).data

/// Read-only `string` from a literal
#define S(literal)                                                                                 \
        (string) { .data = (literal), .len = (int)(sizeof(literal) - 1) }
string string_dup(arena* a, string s);
string string_fmt(arena* a, const char* fmt, ...);
string string_new(arena* a, const char* null, size_t len);
string string_file(arena* a, const char* path);

/// No copy if `mut_s` is last in the arena
char* string_null(arena* a, string mut_s);
/// No copy if contiguous. `mut_s` copied if `mut_base` is last in the arena
string string_cat(arena* a, string mut_base, string mut_s);
string string_insert(arena* a, string base, int index, string s);
string string_repeat(arena* a, string s, int width);

string string_slice(string s, int beg, int end);
bool string_eq(string s1, string s2);
bool string_ieq(string s1, string s2);
bool string_beg(string s, string prefix);
bool string_end(string s, string suffix);

int string_find(string s, string sub);

// Vector ------------------------------------------------------------------------------------------
//
// Hashmap -----------------------------------------------------------------------------------------

#endif // JUST_H
