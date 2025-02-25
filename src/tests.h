#ifdef TESTS // TODO: ifndef
const s8 empty = s8("");
const s8 hello = s8("Hello World!");

TEST("s8cmp") {
        EXPECT(0 == s8cmp(&empty, &empty));
        EXPECT(-1 == s8cmp(&s8("a"), &s8("ab")));
        EXPECT(1 == s8cmp(&s8("abc"), &s8("ab\0")));
}

TEST("s8starts_with") {
        EXPECT(true == s8starts_with(&empty, &empty));
        EXPECT(true == s8starts_with(&hello, &s8("")));
        EXPECT(false == s8starts_with(&s8("Hello World!"), &s8("hell")));
        EXPECT(true == s8starts_with(&s8("Hello World!"), &s8("Hello")));
}

TEST("s8ends_with") {
        EXPECT(true == s8ends_with(&s8(""), &s8("")));
        EXPECT(true == s8ends_with(&s8("Hello World!"), &s8("")));
        EXPECT(false == s8ends_with(&s8("Hello World!"), &s8("old")));
        EXPECT(true == s8ends_with(&s8("Hello World!"), &s8("World!")));
}
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif
