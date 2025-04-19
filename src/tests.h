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
string l = S("l");
string W = S("W");

TEST("string_cmp") {
        EXPECT(string_cmp(Hello, hello) == -1);
        EXPECT(string_cmp(Hello, l) == 1);
}

TEST("string_icmp") {
        EXPECT(string_icmp(Hello, hello) == 0);
        EXPECT(string_icmp(l, W) == -1);
}

TEST("string_startswith") {
        EXPECT(!string_startswith(hello, l));
        EXPECT(string_startswith(hello, S("hello")));
}

TEST("string_endswith") {
        EXPECT(!string_endswith(hello, l));
        EXPECT(string_endswith(hello, S("world!")));
}

TEST("string_cat") {
        u64 before = a->used;
        string base = S(a, "");
        base = string_cat(a, base, W);
        base = string_cat(a, base, W);
        base = string_cat(a, base, W);
        EXPECT(string_eq(base, S("WWW")));
        EXPECT(before + 3 == a->used);
}

TEST("string_inject") {
        EXPECT(string_eq(string_inject(a, string_inject(a, hello, 0, 1, S("H")), 7, 1, W), Hello));
}

arena_destroy(a);
#else
// IWYU pragma: begin_keep
#include "just.h"
// IWYU pragma: end_keep
#endif
