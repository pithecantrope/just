#ifdef TESTS
arena* a = arena_new(1024);

// arena -------------------------------------------------------------------------------------------
TEST("arena") {
        bool* b = alloc(a, bool);
        *b = true;
        alloc(a, s8, 0);
        EXPECT(a->used == sizeof(isize));
        isize* xs = alloc(a, isize, 2);
        EXPECT(a->used == 3 * sizeof(isize));
        xs[0] = -1;
        xs[1] = 0;
        EXPECT(*b == true);
}

TEST("arena_savepoint") {
        usize before = a->used;
        arena_savepoint save = arena_save(a);
        f64* d = alloc(a, f64);
        *d = 3.14;
        arena_restore(save);
        EXPECT(before == a->used);
        arena_reset(a);
}

// s8 ----------------------------------------------------------------------------------------------
const s8* empty = s8("");
const s8* hello = s8("hello world!");
const s8* Hello = s8("Hello World!");
const s8* l = s8("l");
const s8* W = s8("W");

TEST("s8cmp") {
        EXPECT(s8cmp(empty, empty) == 0);
        EXPECT(s8cmp(empty, s8("\0")) == -1);
        EXPECT(s8cmp(l, W) == 1);
}

TEST("s8icmp") {
        EXPECT(s8icmp(hello, Hello) == 0);
        EXPECT(s8icmp(l, W) < 0);
}

TEST("s8starts_with") {
        EXPECT(s8starts_with(empty, empty));
        EXPECT(s8starts_with(hello, empty));
        EXPECT(!s8starts_with(hello, W));
        EXPECT(s8starts_with(hello, s8("hell")));
}

TEST("s8ends_with") {
        EXPECT(s8ends_with(empty, empty));
        EXPECT(s8ends_with(hello, empty));
        EXPECT(!s8ends_with(hello, W));
        EXPECT(s8ends_with(hello, s8("d!")));
}

TEST("s8find") {
        EXPECT(s8find(empty, hello) == -1);
        EXPECT(s8find(hello, empty) == 0);
        EXPECT(s8find(hello, s8("or")) == 7);
}

TEST("s8count") {
        EXPECT(s8count(empty, hello) == 0);
        EXPECT(s8count(hello, empty) == hello->len);
        EXPECT(s8count(hello, l) == 3);
}

TEST("s8cstr") {
        s8* h = s8cstr(a, (const char*)hello->data, (size_t)hello->len);
        EXPECT(h != hello && h->data != hello->data && s8eq(h, hello));
}

TEST("s8copy") {
        s8* h = s8copy(a, hello);
        EXPECT(h != hello && h->data != hello->data && s8eq(h, hello));
}

arena_delete(a);
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
