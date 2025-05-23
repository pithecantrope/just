#include "just.h"

int
main(int argc, char* argv[]) {
        (void)argc;
        (void)argv;
        arena* a = arena_create(1 << 16);
        size_t before;

        // arena -------------------------------------------------------------------------------------------
        arena_savepoint save = arena_save(a);
        alloc(a, char);
        allocn(a, int, 2);
        assert(a->used == 3 * sizeof(int));
        arena_restore(save);
        assert(a->used == 0);

        // string ------------------------------------------------------------------------------------------
        string empty = S(""), s;
        string hello = S("hello, world!"), Hello = S("Hello, World!");
        string l = S("l"), W = S("W");

        // string_fmt
        string_eq(empty, string_fmt(a, ""));
        string_eq(hello, string_fmt(a, "%s, %s!", "hello", "world"));

        // string_cat
        before = a->used;
        s = SA(a, "hello, ");
        assert(string_eq(hello, string_cat(a, s, SA(a, "world!"))));
        assert(before + 13 == a->used);
        assert(string_eq(S("WWW"), string_cat(a, string_cat(a, W, W), W)));
        assert(before + 16 == a->used);

        // string_inject
        assert(string_eq(empty, string_inject(a, empty, 0, 0, empty)));
        assert(string_eq(Hello, string_inject(a, string_inject(a, hello, 0, 1, S("H")), 7, 1, W)));

        // string_ljust
        assert(string_eq(W, string_ljust(a, string_dup(a, W), 1, '!')));
        assert(string_eq(S("l####"), string_ljust(a, string_dup(a, l), 5, '#')));

        // string_z
        before = a->used;
        assert(strcmp("Egor Afanasin", string_z(a, string_new(a, "Egor Afanasin", 13))) == 0);
        assert(before + 14 == a->used);

        // string_istitle
        assert(!string_istitle(l));
        assert(string_istitle(Hello));

        // string_capitalize
        assert(string_eq(empty, string_capitalize(string_dup(a, empty))));
        assert(!string_eq(Hello, string_capitalize(string_dup(a, hello))));

        // string_title
        assert(string_eq(W, string_title(string_dup(a, S("w")))));
        assert(string_eq(Hello, string_title(string_dup(a, hello))));

        arena_destroy(a);
        return EXIT_SUCCESS;
}
