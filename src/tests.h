#ifdef TESTS
arena* a = arena_create(1 << 16);

// arena -------------------------------------------------------------------------------------------
TEST("arena") {
        arena_savepoint save = arena_save(a);
        alloc(a, f32, 3);
        alloc(a, f64);
        EXPECT(a->used == 24);
        arena_restore(save);
        EXPECT(a->used == 0);
}

// s -----------------------------------------------------------------------------------------------

arena_destroy(a);
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
