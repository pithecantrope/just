#include "just.h"

int
main(int argc, char* argv[]) {
        (void)argc;
        (void)argv;
        arena* a = arena_create(1 << 16);
        size_t used;

        // arena -----------------------------------------------------------------------------------
        arena_savepoint save = arena_save(a);
        alloc(a, char);
        allocn(a, int, 2);
        assert(a->used == 3 * sizeof(int));
        arena_restore(save);
        assert(a->used == 0);

        // string ----------------------------------------------------------------------------------
        string empty = S(""), s;
        string hello = S("hello, world!"), Hello = S("Hello, World!");
        // string l = S("l"), W = S("W");
        assert(strcmp("Egor Afanasin", string_str(a, S("Egor Afanasin"))) == 0);

        // string_cat
        used = a->used;
        s = string_cat(a, S("Because"), S(" it"));
        s = string_cat(a, s, S(" just"));
        assert(string_eq(S("Because it just works!"),
                         string_cat(a, s, string_cat(a, S(" works"), S("!")))));
        assert(used + 22 == a->used);

        // string_repeat
        assert(string_eq(S("   "), string_repeat(a, S(" "), 3)));
        assert(string_eq(S("#-#-#"), string_repeat(a, S("#-"), 5)));

        // string_sub
        assert(string_eq(S("world!"), string_sub(hello, 7, -1)));

        (void)Hello;
        (void)empty;
        arena_destroy(a);
        return EXIT_SUCCESS;
}
