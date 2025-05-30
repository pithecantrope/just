#include "just.h"

int
main(int argc, char* argv[]) {
        (void)argc;
        (void)argv;
        arena* a = arena_create(1 << 16);
        // size_t used;

        // Arena -----------------------------------------------------------------------------------
        alloc(a, char);
        allocn(a, int, 2);
        assert(a->used == 3 * sizeof(int));
        arena_reset(a);
        assert(a->used == 0);
        printf(PRIA, FMTA(a));

        // String ----------------------------------------------------------------------------------
        arena_destroy(a);
        return EXIT_SUCCESS;
}
