#ifdef TESTS
const s8* empty = s8("");
const s8* hello = s8("Hello World!");

TEST("s8cmp") {
        EXPECT(0 == s8cmp(empty, empty));
        EXPECT(-1 == s8cmp(empty, s8("\0")));
        EXPECT(1 == s8cmp(hello, s8("Z")));
}

TEST("s8starts_with") {
        EXPECT(true == s8starts_with(empty, empty));
        EXPECT(true == s8starts_with(hello, empty));
        EXPECT(false == s8starts_with(hello, s8("hell")));
        EXPECT(true == s8starts_with(hello, s8("Hello")));
}

TEST("s8ends_with") {
        EXPECT(true == s8ends_with(empty, empty));
        EXPECT(true == s8ends_with(hello, empty));
        EXPECT(false == s8ends_with(hello, s8("old")));
        EXPECT(true == s8ends_with(hello, s8("World!")));
}

TEST("s8find") {
        EXPECT(-1 == s8find(empty, hello));
        EXPECT(0 == s8find(hello, empty));
        EXPECT(7 == s8find(hello, s8("or")));
}

TEST("s8count") {
        EXPECT(0 == s8count(empty, hello));
        EXPECT(12 == s8count(hello, empty));
        EXPECT(3 == s8count(hello, s8("l")));
}
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
