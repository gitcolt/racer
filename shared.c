#include "shared.h"

void draw_circle_lines_thickness(V2 center, float radius, float thickness, Color color) {
    for (int i = 0; i < thickness; ++i)
        DrawCircleLinesV(center, radius - i, color);
}
