#include "tests.h"

int
main(int argc, char* argv[]) {
        TODO(argc, argv);

        const char* TEST;
#define TESTS
#include "tests.h"
#undef TESTS

        return EXIT_SUCCESS;
}
