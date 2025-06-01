#include "just.h"

int
main(int argc, char* argv[]) {
        (void)argc;
        (void)argv;
        arena* a = arena_create(1 << 16);
        size_t used;

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

        // string_new
        used = a->used;
        assert(strcmp("Egor Afanasin", string_null(a, string_new(a, "Egor Afanasin", 13))) == 0);
        assert(used + 14 == a->used);

        // string_file
        string f = string_file(a, "test.txt");
        assert(string_eq(f, S("Egor Afanasin\njust\nBecause it just works\n")));

        // Vector ----------------------------------------------------------------------------------

        // Hashmap ---------------------------------------------------------------------------------

        arena_destroy(a);
        return EXIT_SUCCESS;
}
