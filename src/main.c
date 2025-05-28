#include "just.h"

int
main(int argc, char* argv[]) {
        (void)argc;
        (void)argv;
        arena* a = arena_create(1 << 16);

        // arena -----------------------------------------------------------------------------------
        arena_savepoint save = arena_save(a);
        alloc(a, char);
        allocn(a, int, 2);
        assert(a->used == 3 * sizeof(int));
        arena_restore(save);
        assert(a->used == 0);

        // string ----------------------------------------------------------------------------------
        assert(strcmp("Egor Afanasin", string_str(a, S("Egor Afanasin"))) == 0);

        arena_destroy(a);
        return EXIT_SUCCESS;
}
