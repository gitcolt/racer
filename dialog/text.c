#include "text.h"
#include "../shared/mymath.h"
#include "../shared/shared.h"

#include <string.h>
#include <stdio.h>

int draw_text_rec(const char *text, Rectangle rec, Font font, Color color) {
    char *full_text = strdup(text);

    V2 text_size = MeasureTextEx(font, text, FONT_SIZE, HORIZ_SPACING);
    int text_len = TextLength(full_text);
    float char_width = text_size.x / text_len;
    int max_chars_per_line = rec.width / (char_width);
    char *line_start = &full_text[0];
    char *line_end;
    if (&full_text[text_len] < &line_start[max_chars_per_line])
        line_end = &full_text[text_len];
    else
        line_end = &line_start[max_chars_per_line];
    int pos_y = rec.y;
    while (true) {
        if (*line_start == '\0') {
            break;
        }
        while (*line_end != ' ' && *line_end != '\0')
            --line_end;
        if (line_end <= line_start) {
            break;
        }
        if ((line_end - full_text) > text_len) {
            break;
        }

        *line_end = '\0';
        DrawTextPro(font,
                    line_start,
                    (V2){rec.x, pos_y},
                    (V2){0,0},
                    0,
                    FONT_SIZE,
                    HORIZ_SPACING,
                    color);

        line_start = line_end + 1;
        line_end = MIN(&line_start[max_chars_per_line], &full_text[text_len]);
        pos_y += text_size.y;
    }

    return pos_y;
}
