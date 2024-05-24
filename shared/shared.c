#include "shared.h"
#include "ctype.h"

#include "rlgl.h"

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

void draw_textured_quad(Texture2D tex, V2 center) {
    rlSetTexture(tex.id);
    rlBegin(RL_QUADS);
        rlColor4ub(255, 255, 255, 255);
        // rlNormal3f(0, 0, 1.0);

        rlTexCoord2f(0, 0);
        rlVertex3f(center.x - 10, 0, center.y - 10);

        rlTexCoord2f(1, 0);
        rlVertex3f(center.x - 10, 0, center.y + 10);

        rlTexCoord2f(1, 1);
        rlVertex3f(center.x + 10, 0, center.y + 10);

        rlTexCoord2f(0, 1);
        rlVertex3f(center.x + 10, 0, center.y - 10);

    rlEnd();
    rlSetTexture(0);
}
