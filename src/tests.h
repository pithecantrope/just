#ifdef TESTS
arena* a = arena_create(1 << 16);

// arena -------------------------------------------------------------------------------------------
TEST("arena") {
        arena_savepoint save = arena_save(a);
        alloc(a, f32, 3);
        alloc(a, f64);
        EXPECT(a->used == 3 * 8);
        arena_restore(save);
        EXPECT(a->used == 0);
}

// string ------------------------------------------------------------------------------------------
string hello = S("hello, world!");
string Hello = S("Hello, World!");
// string l = S("l");
string W = S("W"); 

TEST("string_inject") {
        string s = string_inject(a, hello, 7, 1, W);
        printf(PRI_string "\n", FMT_string(s));
        printf(PRI_arena  "\n", FMT_arena(a));
        string cat = string_cat(a, s, Hello);
        printf(PRI_string "\n", FMT_string(cat));
        printf(PRI_arena  "\n", FMT_arena(a));
}

arena_destroy(a);
#else
// IWYU pragma: begin_keep
#include "just.h"
// IWYU pragma: end_keep
#endif
