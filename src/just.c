#include "just.h"

// arena -------------------------------------------------------------------------------------------
arena*
arena_create(usize capacity) {
        assert(sizeof(arena) <= SIZE_MAX - capacity);
        arena* a = malloc(sizeof(arena) + capacity);
        assert(a != NULL);
        *a = (arena){.cap = capacity};
        return a;
}

void*
arena_alloc(arena* a, usize align, usize size, usize count) {
        assert(a != NULL && "Invalid arena");
        assert(size != 0 && "Invalid size");
        assert(IS_POW2(align) && "Invalid alignment");
        usize padding = -(uintptr_t)(a->data + a->used) & (align - 1);
        assert(count <= (a->cap - a->used - padding) / size && "Increase arena capacity");
        void* ptr = a->data + a->used + padding;
        a->used += padding + count * size;
        return ptr;
}

// Pseudo-random -----------------------------------------------------------------------------------
#include <time.h>
static u64 rngstate[4];

static inline u64
rngrotl(u64 x, int k) {
        return (x << k) | (x >> (64 - k));
}

static u64
rng(void) {
        static bool seeded = false;
        if (!seeded) {
                u64 seed = (u64)(time(NULL) + clock());
                u64 tmp;
                for (int i = 0; i < 4; ++i) {
                        seed += 0x9e3779b97f4a7c15;
                        tmp = (seed ^ (seed >> 30)) * 0xbf58476d1ce4e5b9;
                        tmp = (tmp ^ (tmp >> 27)) * 0x94d049bb133111eb;
                        rngstate[i] = tmp ^ (tmp >> 31);
                }
                seeded = true;
        }

        const u64 result = rngrotl(rngstate[0] + rngstate[3], 23) + rngstate[0];
        const u64 tmp = rngstate[1] << 17;
        rngstate[2] ^= rngstate[0];
        rngstate[3] ^= rngstate[1];
        rngstate[1] ^= rngstate[2];
        rngstate[0] ^= rngstate[3];
        rngstate[2] ^= tmp;
        rngstate[3] = rngrotl(rngstate[3], 45);
        return result;
}

f64
rng01(void) {
        return (f64)(rng() >> 11) * 0x1.0p-53;
}
