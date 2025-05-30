/**
 * @file just.h
 * @brief String, Vector and Hashmap backed by arena
 * 
 Group many allocations into a single common case;
 * Arena is a simple linear allocator tied to object lifetime. These "containers"
NO manually managing memory (management)
 * do not own the objects, and so do not require memory management, which
 * is why they're so simple. Instead, memory regions own objects, which
 * solves a bunch of problems at once. It means:
 * Faster than malloc. Impossible to memory leak.
large number of allocations can be bucketed into the same arena
which offers lightning fast allocation and deallocation, eliminating per-allocation freeing
requirements, first being proved out by the stack, and * 
 * @author Egor Afanasin <afanasin.egor@gmail.com>
A very high level description of an arena is “a handle to which allocations are bound”. When
an allocation occurs, it is “bound” to an “arena handle”. This makes it easily expressible to, for
 instance, clear all allocations “bound” to an “arena handle”.
It is trivial, then, to identify which functions are performing allocations.
And because, in an API like the above, the arena is a required parameter, the
caller must choose an arena, and thus determine the lifetime of any persistent
allocations.
As I’ve mentioned, a key concept behind the arena is grouping lifetimes together. 
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
typedef struct {
        size_t used, cap;
        char* data;
} arena;

// printf(PRIA, FMTA(arena));
#define PRIA        "{used:%zu, cap:%zu, data:%p}"
#define FMTA(arena) (arena)->used, (arena)->cap, (void*)(arena)->data

// arene_alloc/release?
arena* arena_create(size_t capacity);
void arena_reset(arena* a); // or arena_clear
void arena_destroy(arena* a);
// use alloc and allocn macros
void* arena_alloc(arena* a, size_t align, size_t size, size_t count);
#define alloc(arena, type)     (type*)arena_alloc(arena, alignof(type), sizeof(type), 1)
#define allocn(arena, type, n) (type*)arena_alloc(arena, alignof(type), sizeof(type), (size_t)(n))


// This is where per-thread scratch arenas become useful. These are simply thread-local arenas, which can be retrieved at any time. They can then be used with the “temporary sub-lifetime” trick I introduced earlier, with an additional API layered over the core arena API:

// struct ArenaTemp
// {
//   Arena *arena;
//   U64 pos;
// };

// ArenaTemp ArenaTempBegin(Arena *arena); // grabs arena's position
// void ArenaTempEnd(ArenaTemp temp);      // restores arena's position

// how to implement vector??? or hash??? stuff that changes
// maybe back to region_arena???


// Dynamic Array Arenas As I’ll explain in the Implementation Notes, an arena
// implementation can have a strategy for growing and shrinking (while not
// relocating existing allocations). This makes them useful for storing collections
// of information where the size is not known upfront. Depending on the exact
// growth strategy, this either makes arenas a suitable replacement for dynamic
// arrays, or a perfect implementation of one. Because the specifics of this
// style of arena usage are tightly coupled with implementation details, I’ll just
// mention this for now, and hopefully the Implementation Notes will illuminate the
// parts I’m glossing over at the moment.
// 
//  Implementation Notes I’ve already introduced the basic mechanism for
// implementing an arena, when I described how the stack works. An arena works
// in precisely the same way (although you can play with the details—for example,
// auto-aligning allocations). There is one aspect I’ve not yet covered, however,
// which is what happens when the arena runs out of storage. There are a number
// of strategies one might employ in tackling this scenario, depending on their
// particular case. One strategy is to simply pre-allocate a single fixed-size
// block of memory for an arena, and abort when storage runs out, because the
// project has strict memory usage requirements (if it’s, for example, on an
// embedded device). The more common case, though, is that you’re writing code for,
// say, a modern consumer desktop computer, or a modern game console. In this case,
// it’s much easier to have a strategy for growing the arena’s storage when it runs
// out. One strategy is to spread the arena across a variably-sized linked list of
// large blocks. If there is not enough room for a new allocation on an arena, your
// implementation falls back to asking the operating system for a new block, and
// then begins allocating on the new block. When the arena’s allocation position
// is pushed far back enough, it can return to the previous block. This notably,
// eliminates the guarantee of memory contiguity within the arena, which makes the
// arena unusable for implementing a dynamic array, because a dynamic array can
// be trivially accessed with a single offset into a single block. In this case,
// to access an arbitrary offset in the storage, you first would need to scan
// the chain of blocks. This is likely not prohibitively expensive, and the exact
// performance characteristics can be tweaked by changing the block size—but it
// does mean using it is more nuanced than just a single block of memory. Another
// fancier strategy is to take advantage of modern MMUs and 64-bit CPUs (which are
// virtually ubiquitous these days). On modern PCs, for instance, it’s likely that
// you have 48 bits (256 terabytes!) of virtual address space at your disposal.
// This means, functionally, that you can still have arenas both grow dynamically,
// and work with just a single block (and thus maintain the guarantee of memory
// contiguity). Instead of committing physical storage for the entire block upfront
// (e.g. by using malloc for the storage), the implementation simply reserves
// the address range in your virtual address space (e.g. by using VirtualAlloc on
// Windows). Then, when the arena reaches a new page in the virtual address space
// range that has not been backed by physical memory, it will request more physical
// memory from the operating system. In this case, you still must decide on an
// upper-bound for your arena storage, but because of the power of exponentiation,
// this upper-bound can be ridiculously large (say, 64 gigabytes). So, in short,
// reserve a massive upper-bound of contiguous virtual address space, then commit
// physical pages as needed. Finally, it’s easy to compose all of the above
// strategies, or make them all available in various scenarios, through the same
// API. So, don’t assume they’re mutually exclusive! Before choosing any strategy,
// carefully consider which platforms you’re writing for, and the real constraints
// on your solution. For example, my understanding is that the Nintendo Switch has
// 38 bits of virtual address space. The magic growing arena can still work here,
// but you’ll have a tighter constraint on address space.
// String ------------------------------------------------------------------------------------------
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

// .data .len is bullshit
#endif // JUST_H
