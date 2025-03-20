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

TEST("s8starts_with") {
        EXPECT(!s8starts_with(hello, l));
        EXPECT(s8starts_with(hello, s8("hello")));
}

TEST("s8ends_with") {
        EXPECT(!s8ends_with(hello, l));
        EXPECT(s8ends_with(hello, s8("world!")));
}

TEST("s8slice") {
        EXPECT(s8eq(s8slice(a, Hello, 0, Hello.len, 2), s8("HloWrd")));
        EXPECT(s8eq(s8slice(a, Hello, Hello.len - 1, -1, -1), s8("!dlroW olleH")));
}

TEST("s8repeat") {
        EXPECT(s8eq(s8repeat(a, l, 0), empty));
        EXPECT(s8eq(s8repeat(a, W, 3), s8("WWW")));
}

TEST("s8cat") {
        EXPECT(s8eq(s8cat(a, empty, empty), empty));
        EXPECT(s8eq(s8cat(a, l, W), s8("lW")));
}

TEST("s8inject") {
        EXPECT(s8eq(s8inject(a, l, 0, 1, W), W));
        EXPECT(s8eq(s8inject(a, Hello, 5, 0, s8(",")), s8("Hello, World!")));
}

TEST("s8span") {
        EXPECT(s8eq(s8span(W, 0, W.len), W));
        EXPECT(s8eq(s8span(Hello, 6, 5), s8("World")));
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
        indexes arr = s8findall(a, hello, empty);
        EXPECT(arr.len == hello.len && arr.data[arr.len - 1] == hello.len - 1);
        arr = s8findall(a, hello, l);
        EXPECT(arr.len == 3 && arr.data[0] == 2 && arr.data[1] == 3 && arr.data[2] == 9);
}

TEST("s8is_title") {
        EXPECT(!s8is_title(s8("H_ello, World!")));
        EXPECT(s8is_title(W) && s8is_title(Hello));
}

TEST("s8capitalize") {
        EXPECT(!s8eq(s8capitalize(s8dup(a, Hello)), Hello));
        EXPECT(s8eq(s8capitalize(s8dup(a, empty)), empty));
}

TEST("s8title") {
        EXPECT(s8eq(s8title(S8(a, "w")), W));
        EXPECT(s8eq(s8title(s8dup(a, hello)), Hello));
}

arena_destroy(a);
#else
// IWYU pragma: begin_keep
#include "just.h"
// IWYU pragma: end_keep
#endif
