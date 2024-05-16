#ifndef STR_UTILS_H
#define STR_UTILS_H

#define DYN_ARR_IMPL
#include "dyn_arr.h"
#include "string.h"

// rtrim() and trim() modify the string in-place
char *ltrim(char *str);
char *rtrim(char *str);
char *trim(char *str);

typedef struct {
    char *s;
    size_t str_len;
} StringBuilder;

void sb_create(StringBuilder *sb);
void sb_destroy(StringBuilder *sb);
void sb_append(StringBuilder *sb, const char *str);
void sb_build(StringBuilder *sb, char *out);

#endif // end STR_UTILS_H

#ifdef STR_UTILS_IMPL

#include <string.h>
#include <ctype.h>

char *ltrim(char *str) {
    while (isspace(*str))
        ++str;
    return str;
}

char *rtrim(char *str) {
    char *p = str + strlen(str) - 1;
    while (isspace(*p) && p > str)
        --p;
    *(p + 1) = '\0';
    return str;
}

char *trim(char *str) {
    return rtrim(ltrim(str));
}

void sb_create(StringBuilder *sb) {
    sb->s = ARR(char, 0);
}

void sb_destroy(StringBuilder *sb) {
    arr_free(sb->s);
}

void sb_append(StringBuilder *sb, const char *str) {
    size_t str_len = strlen(str);
    for (size_t i = 0; i < str_len; ++i)
        ARR_APPEND(sb->s, str[i]);
    sb->str_len = str_len + 1;
}

void sb_build(StringBuilder *sb, char *out) {
    ARR_APPEND(sb->s, '\0');
    strcpy(out, sb->s);
}

#endif // end implementation
