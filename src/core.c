#include "core.h"

// arena -------------------------------------------------------------------------------------------

// s8 ----------------------------------------------------------------------------------------------
i32
s8cmp(const s8 s1 PTR, const s8 s2 PTR) {
        if (s1->len != s2->len) {
                return s1->len < s2->len ? -1 : 1;
        }
        i32 cmp = memcmp(s1->data, s2->data, (size_t)s1->len);
        return (cmp > 0) - (cmp < 0);
}

inline bool
s8eq(const s8 s1 PTR, const s8 s2 PTR) {
        return s8cmp(s1, s2) == 0;
}

bool
s8starts_with(const s8 s PTR, const s8 prefix PTR) {
        return (s->len >= prefix->len) && memcmp(s->data, prefix->data, (size_t)prefix->len) == 0;
}

bool
s8ends_with(const s8 s PTR, const s8 suffix PTR) {
        return (s->len >= suffix->len)
               && memcmp(s->data + (s->len - suffix->len), suffix->data, (size_t)suffix->len) == 0;
}

// Horspool algorithm
isize
s8find(const s8 s PTR, const s8 sub PTR) {
        if (s->len < sub->len) {
                return -1;
        }
        if (sub->len == 0) {
                return 0;
        }
        isize last_occ[U8ALPHABET];
        memset(last_occ, -1, U8SIZE * U8ALPHABET);
        for (isize i = 0; i < sub->len - 1; ++i) {
                last_occ[sub->data[i]] = i;
        }

        for (isize i = 0; i <= s->len - sub->len;) {
                for (isize j = sub->len - 1; sub->data[j] == s->data[j + i];) {
                        if (--j == -1) {
                                return i;
                        }
                }
                i += sub->len - 1 - last_occ[s->data[i + sub->len - 1]];
        }
        return -1;
}

isize
s8count(const s8 s PTR, const s8 sub PTR) {
        if (s->len < sub->len) {
                return 0;
        }
        if (sub->len == 0) {
                return s->len;
        }
        isize count = 0;
        isize last_occ[U8ALPHABET];
        memset(last_occ, -1, U8SIZE * U8ALPHABET);
        for (isize i = 0; i < sub->len - 1; ++i) {
                last_occ[sub->data[i]] = i;
        }

        for (isize i = 0; i <= s->len - sub->len;) {
                for (isize j = sub->len - 1; sub->data[j] == s->data[j + i];) {
                        if (--j == -1) {
                                ++count;
                                break;
                        }
                }
                i += sub->len - 1 - last_occ[s->data[i + sub->len - 1]];
        }
        return count;
}

// Internals ---------------------------------------------------------------------------------------
#if defined(__SANITIZE_ADDRESS__)   || __has_feature(address_sanitizer)
const char* __asan_default_options (void) { return "abort_on_error=true:check_initialization_order=true:strict_init_order=true:detect_stack_use_after_return=true:strict_string_checks=true"; }
#endif
#if defined(__SANITIZE_UNDEFINED__) || __has_feature(undefined_behavior_sanitizer)
const char* __ubsan_default_options(void) { return "abort_on_error=true"; }
#endif
