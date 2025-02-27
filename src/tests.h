#ifdef TESTS
arena a = {0};

TEST("arena_alloc") {
        arena_alloc(&a, bool);
        arena_alloc(&a, s8, 0);
        EXPECT(a.beg == a.end && a.beg->used == sizeof(usize));
        arena_alloc_aligned(&a, 8 + ARENA_REGION_CAPACITY, alignof(uintptr_t), 1);
        EXPECT(a.beg != a.end);
        usize* pusize = arena_alloc(&a, usize);
        EXPECT(a.beg->used == 2 * sizeof(usize));
        *pusize = 19;
}

TEST("arena_reset") {
        arena_reset(&a);
        EXPECT(a.beg != a.end && a.beg->used == 0 && a.end->used == 0);
}

const s8* empty = s8("");
const s8* hello = s8("Hello World!");

TEST("s8cmp") {
        EXPECT(s8cmp(empty, empty) == 0);
        EXPECT(s8cmp(empty, s8("\0")) < 0);
        EXPECT(s8cmp(hello, s8("Z")) > 0);
}

TEST("s8starts_with") {
        EXPECT(s8starts_with(empty, empty) == true);
        EXPECT(s8starts_with(hello, empty) == true);
        EXPECT(s8starts_with(hello, s8("hell")) == false);
        EXPECT(s8starts_with(hello, s8("Hello")) == true);
}

TEST("s8ends_with") {
        EXPECT(s8ends_with(empty, empty) == true);
        EXPECT(s8ends_with(hello, empty) == true);
        EXPECT(s8ends_with(hello, s8("old")) == false);
        EXPECT(s8ends_with(hello, s8("World!")) == true);
}

TEST("s8find") {
        EXPECT(s8find(empty, hello) == -1);
        EXPECT(s8find(hello, empty) == 0);
        EXPECT(s8find(hello, s8("or")) == 7);
}

TEST("s8count") {
        EXPECT(s8count(empty, hello) == 0);
        EXPECT(s8count(hello, empty) == 12);
        EXPECT(s8count(hello, s8("l")) == 3);
}

arena_free(&a);
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
