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
 * Because allocations are simple pointer bumps inside a lifetime-bound
 * memory region, it is significantly faster than traditional malloc/free.
 *
 * @note Uses `assert()` for error handling. No return values need checking.
 *
 * @author Egor Afanasin <afanasin.egor@gmail.com>
 */

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

// Arena -------------------------------------------------------------------------------------------
/**
 * Since the arena must be explicitly provided to each allocation function,
 * callers are forced to consider object lifetime from the start — making it
 * trivial to track where allocations occur and who owns them.
 *
 * This arena has a fixed capacity and does not grow — always allocate 
 * enough memory upfront. Note that allocations cannot be freed
 * individually — to reclaim memory, reset or destroy the entire arena.
 */

// A linear allocator tied to a specific lifetime
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

// Prefer `alloc()` and `allocn()` macros instead
void* arena_alloc(arena* a, size_t align, size_t size, size_t count);
#define alloc(arena, type)     (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define allocn(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), (size_t)(n))

// String ------------------------------------------------------------------------------------------
//
// Vector ------------------------------------------------------------------------------------------
//
// Resizes always change the backing array address, and the old array remains
// valid. This is also just like slices in Go. So just shitting memory
// Finally the updated replica is copied over the original slice header,
// updating it with the new data pointer and capacity. The original backing array
// is untouched but is no longer referenced through this slice header. Old slice
// headers will continue to function with the old backing array, such as when the
// arena is reset to a point where the dynamic array was smaller.
// In practice, a dynamic array comes from old backing arrays whose total size
// adds up just shy of the current array capacity. For example, if the current
// capacity is 16, old arrays are size 2+4+8 = 14.
//
// Hashmap -----------------------------------------------------------------------------------------

#endif // JUST_H
