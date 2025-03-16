#ifdef TESTS
arena* a = arena_new(1 << 16);

// arena -------------------------------------------------------------------------------------------
TEST("arena") {
        alloc(a, bool);
        alloc(a, bool);
        EXPECT(a->used == 2);
}

// TEST("arena_savepoint") {
//         usize before = a->used;
//         arena_savepoint save = arena_save(a);
//         f64* d = alloc(a, f64);
//         *d = 3.14;
//         arena_restore(save);
//         EXPECT(before == a->used);
//         arena_reset(a);
// }

arena_delete(a);
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
