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
        string empty = S("");
        string Hello = S("Hello, World!"), hello = S("hello, world!");

        // string_fmt
        assert(string_eq(hello, string_fmt(a, "hello, world!")));
        assert(string_eq(Hello, string_fmt(a, "%s, %s!", "Hello", "World")));

        // string_new
        assert(string_eq(empty, string_new(a, "", 0)));
        assert(strcmp("Egor Afanasin", string_null(a, string_new(a, "Egor Afanasin", 13))) == 0);

        // Vector ----------------------------------------------------------------------------------

        // Hashmap ---------------------------------------------------------------------------------

        arena_destroy(a);
        return EXIT_SUCCESS;
}
