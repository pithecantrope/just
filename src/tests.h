#ifdef TESTS
arena a = {0};
const s8* empty = s8("");
const s8* hello = s8("Hello World!");

TEST("arena_save") {
        arena_savepoint save = arena_save(&a);
        arena_restore(&save);
        EXPECT(a.regions == 0 && a.head == NULL);
}

TEST("arena_alloc") {
        alloc(&a, bool);
        alloc(&a, s8, 0);
        EXPECT(a.regions == 1 && a.head->used == sizeof(usize));
        arena_alloc(&a, 8 + ARENA_REGION_CAPACITY, alignof(uintptr_t), 1);
        EXPECT(a.regions == 2);
        usize* pusize = alloc(&a, usize);
        EXPECT(a.head->next->used == 2 * sizeof(usize));
        *pusize = 19;
}

TEST("arena_restore") {
        usize before = a.head->next->used;
        arena_savepoint s = arena_save(&a);
        s8* ps = alloc(&a, s8);
        *ps = *hello;
        arena_restore(&s);
        EXPECT(before == a.head->next->used);
}

TEST("arena_reset") {
        arena_reset(&a);
        EXPECT(a.regions == 2 && a.head->used == 0 && a.head->next->used == 0);
}

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

TEST("arena_free") {
        arena_free(&a);
        EXPECT(a.regions == 0);
}
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
