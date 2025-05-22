#include "just.h"

int
main(int argc, char* argv[]) {
        (void)argc;
        (void)argv;
        arena* a = arena_create(1 << 16);

        // arena -------------------------------------------------------------------------------------------
        arena_savepoint save = arena_save(a);
        alloc(a, char);
        allocn(a, int, 2);
        assert(a->used == 3 * sizeof(int));
        arena_restore(save);
        assert(a->used == 0);

        // string ------------------------------------------------------------------------------------------
        string empty = S("");
        string hello = S("hello, world!");
        string Hello = S("Hello, World!");
        // string l = S("l");
        string W = S("W");

        // string_istitle
        assert(!string_istitle(hello));
        assert(string_istitle(Hello));

        // string_capitalize
        assert(string_eq(W, string_capitalize(string_dup(a, S("w")))));
        assert(!string_eq(Hello, string_capitalize(string_dup(a, Hello))));

        // string_title
        assert(string_eq(empty, string_title(empty)));
        assert(string_eq(Hello, string_title(string_dup(a, hello))));

        arena_destroy(a);
        return EXIT_SUCCESS;
}
