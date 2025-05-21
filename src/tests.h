#ifdef TESTS
arena* a = arena_create(1 << 16);

// arena -------------------------------------------------------------------------------------------
TEST("arena") {
        arena_savepoint save = arena_save(a);
        alloc(a, char);
        allocn(a, int, 2);
        EXPECT(a->used == 3 * sizeof(int));
        arena_restore(save);
        EXPECT(a->used == 0);
}

// string ------------------------------------------------------------------------------------------
// string empty = S("");
string hello = S("hello, world!");
string Hello = S("Hello, World!");
// string l = S("l");
// string W = S("W");

TEST("string_istitle") {
        EXPECT(!string_istitle(hello));
        EXPECT(string_istitle(Hello));
}

arena_destroy(a);
#else
// IWYU pragma: begin_keep
#include "just.h"
// IWYU pragma: end_keep
#endif
