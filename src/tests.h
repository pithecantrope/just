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

// s8 ----------------------------------------------------------------------------------------------
s8 empty = s8("");
s8 hello = s8("hello world!");
s8 Hello = s8("Hello World!");
s8 l = s8("l");
s8 W = s8("W");

TEST("s8cmp") {
        EXPECT(s8cmp(empty, empty) == 0);
        EXPECT(s8cmp(Hello, hello) == -1);
        EXPECT(s8cmp(Hello, l) == 1);
}

TEST("s8icmp") {
        EXPECT(s8icmp(Hello, hello) == 0);
        EXPECT(s8icmp(l, W) == -1);
}

TEST("s8find") {
        EXPECT(s8find(empty, hello) == -1);
        EXPECT(s8find(hello, empty) == 0);
        EXPECT(s8find(hello, l) == 2);
}

TEST("s8count") {
        EXPECT(s8count(empty, hello) == 0);
        EXPECT(s8count(hello, empty) == hello.len);
        EXPECT(s8count(hello, l) == 3);
}

TEST("s8findall") {
        EXPECT(s8findall(a, empty, hello).len == 0);
        isizes index = s8findall(a, hello, empty);
        EXPECT(index.len == hello.len && index.data[index.len - 1] == hello.len - 1);
        index = s8findall(a, hello, l);
        EXPECT(index.len == 3 && index.data[0] == 2 && index.data[1] == 3 && index.data[2] == 9);
}

arena_destroy(a);
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
