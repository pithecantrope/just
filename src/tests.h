#ifdef TESTS
arena* a = arena_new(1024);
const s8* empty = s8("");
const s8* hello = s8("Hello World!");

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
TEST("s8cmp") {
        EXPECT(s8cmp(empty, empty) == 0);
        EXPECT(s8cmp(empty, s8("\0")) == -1);
        EXPECT(s8cmp(hello, s8("Z")) == 1);
}

// TEST("s8capitalize") {
//         s8* h = s8cstr(a, "hello world!", 12);
//         s8capitalize(h);
//         printf("%.*s\n", (int)h->len, h->data);
//         // EXPECT(s8eq(h, hello));
// }

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

arena_delete(a);
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
