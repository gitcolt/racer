#include "dialog.h"
#include "../text.h"
#include "../colors.h"
#include "../shared.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const float border_size = 15;
static const float text_margin = 20;

void dialog_init(Dialog *dialog) {
    ZERO_MEM(dialog);
    memset(dialog, 0, sizeof(*dialog));
    dialog->font = GetFontDefault();
    memset(dialog->text, 0, sizeof(*dialog->text));
    memset(dialog->next, 0, sizeof(*dialog->next));
}

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
    int text_end_y = draw_text_rec(dialog->text, rec, dialog->font);

    if (dialog->num_choices) {
        rec.y = text_end_y + 20;
        for (int i = 0; i < dialog->num_choices; ++i) {
            Choice *choice = &dialog->choices[i];
            char *buf;
            asprintf(&buf, ">  %s", choice->text);
            draw_text_rec(buf, rec, dialog->font);
            free(buf);
            // draw_text_rec(choice->text, rec, dialog->font);
            rec.y += 30;
        }
    }
}

void var_set_int(Var *var, int val) {
    sprintf(var->val_str, "%d", val);
}

void var_set_str(Var *var, const char *val) {
    strcpy(var->val_str, val);
}
