#ifndef TEXT_H
#define TEXT_H

#include "raylib.h"

#define FONT_SIZE 20
#define HORIZ_SPACING 1
#define VERT_SPACING 1

void draw_text_rec(const char *text, Rectangle rec, Font font);

#endif
