#include "tests.h"

int
main(void) {
        const char* TEST;
#define TESTS
#include "tests.h"
#undef TESTS
        return EXIT_SUCCESS;
}
