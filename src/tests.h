#if defined (TESTS)
TEST("true") {
        EXPECT(!0);
}

TEST("false") {
        EXPECT(!0 == 0);
}
#else
// IWYU pragma: begin_keep
#include "core.h"
// IWYU pragma: end_keep
#endif

