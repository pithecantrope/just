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

        // String ----------------------------------------------------------------------------------
        string Hello = S("Hello, World!"), hello = S("hello, world!");

        // string_fmt
        assert(string_eq(hello, string_fmt(a, "hello, world!")));
        assert(string_eq(Hello, string_fmt(a, "%s, %s!", "Hello", "World")));

        // Vector ----------------------------------------------------------------------------------

        // Hashmap ---------------------------------------------------------------------------------

        arena_destroy(a);
        return EXIT_SUCCESS;
}
