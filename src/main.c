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
        before = a->used;
        assert(string_eq(S("l####"), string_ljust(a, string_dup(a, l), 5, '#')));
        assert(before + 5 == a->used);

        // string_rjust
        assert(string_eq(W, string_rjust(a, W, 1, '!')));
        assert(string_eq(S("****l"), string_rjust(a, l, 5, '*')));

        // string_center
        assert(string_eq(S(" W "), string_center(a, W, 3, ' ')));
        assert(string_eq(S("W!"), string_center(a, W, 2, '!')));

        // string_file
        string f = string_file(a, "test.txt");
        assert(string_eq(f, S("Egor Afanasin\njust\nBecause it just works\n")));

        // string_z
        before = a->used;
        assert(strcmp("Egor Afanasin", string_z(a, string_new(a, "Egor Afanasin", 13))) == 0);
        assert(before + 14 == a->used);

        // string_cmp
        assert(string_cmp(Hello, hello) == -1);
        assert(string_cmp(l, W) == 1);

        // string_icmp
        assert(string_icmp(Hello, hello) == 0);
        assert(string_icmp(l, W) == -1);

        // string_ieq
        assert(!string_ieq(l, W));
        assert(string_ieq(Hello, hello));

        // string_startswith
        assert(string_startswith(empty, empty));
        assert(string_startswith(f, S("Egor Afanasin")));

        // string_endswith
        assert(string_endswith(W, empty));
        assert(string_endswith(f, S("works\n")));

        // string_in
        assert(string_in(empty, empty));
        assert(string_in(S("\xFF"), S("\xFF")));
        assert(string_in(S("just"), f));

        // TODO:

        // string_istitle
        assert(!string_istitle(l));
        assert(string_istitle(Hello));

        // string_capitalize
        assert(string_eq(empty, string_capitalize(string_dup(a, empty))));
        s = SA(a, "w");
        string_capitalize(s);
        assert(string_eq(W, s));

        // string_title
        assert(string_eq(W, string_title(string_dup(a, S("w")))));
        assert(string_eq(Hello, string_title(string_dup(a, hello))));

        arena_destroy(a);
        return EXIT_SUCCESS;
}
