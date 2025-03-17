#ifdef TESTS
arena* a = arena_create(1 << 16);
s8 empty = s8("");
s8 hello = s8("hello world!");
s8 Hello = s8("Hello World!");
s8 l = s8("l");
// s8 W = s8("W");

// arena -------------------------------------------------------------------------------------------
TEST("arena") {
        arena_savepoint save = arena_save(a);
        alloc(a, f32, 3);
        alloc(a, f64);
        EXPECT(a->used == 24);
        arena_restore(save);
        EXPECT(a->used == 0);
}

// s8 ----------------------------------------------------------------------------------------------
TEST("s8cmp") {
        EXPECT(s8cmp(empty, empty) == 0);
        EXPECT(s8cmp(Hello, hello) == -1);       
        EXPECT(s8cmp(Hello, l) == 1);       
}

arena_destroy(a);
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
