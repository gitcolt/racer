#include "dialog.h"
#include "../text.h"
#include "../colors.h"

#include <string.h>
#include <stdio.h>

static const float border_size = 15;
static const float text_margin = 20;

void dialog_show(Dialog *dialog, Rectangle rec) {
    DrawRectangleRec(rec, COLOR_DIALOG_BORDER);

    rec.width -= border_size;
    rec.height -= border_size;
    rec.x += border_size/2;
    rec.y += border_size/2;
    DrawRectangleRec(rec, COLOR_DIALOG_BG);

    rec.width -= text_margin;
    rec.height -= text_margin;
    rec.x += text_margin/2;
    rec.y += text_margin/2;
    draw_text_rec(dialog->text, rec, dialog->font);
}
