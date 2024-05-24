#include "shared.h"
#include "ctype.h"

#include <string.h>

void draw_circle_lines_thickness(V2 center, float radius, float thickness, Color color) {
    for (int i = 0; i < thickness; ++i)
        DrawCircleLinesV(center, radius - i, color);
}

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

