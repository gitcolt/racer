#include "dialog.h"
#include "text.h"
#include "../shared/colors.h"
#include "../shared/shared.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const float border_size = 15;
static const float text_margin = 20;

Conversation *get_convo_by_id(Conversations *convos, const char *convo_id) {
    for (int i = 0; i < convos->num_convos; ++i) {
        if (!strcmp(convo_id, convos->convos[i].id))
            return &convos->convos[i];
    }
    return NULL;
}

void conversations_init(Conversations *convos) {
    memset(convos, 0, sizeof(*convos));
}

void conversation_init(Conversation *convo) {
    memset(convo, 0, sizeof(*convo));
}

Dialog *get_dialog_by_id(Conversation *convo, const char *dialog_id) {
    for (int i = 0; i < convo->num_dialogs; ++i) {
        if (!strcmp(dialog_id, convo->dialogs[i].id))
            return &convo->dialogs[i];
    }
    return NULL;
}

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
    int text_end_y = draw_text_rec(dialog->text, rec, dialog->font, BLACK);

    if (dialog->num_choices) {
        rec.y = text_end_y + 20;
        for (int i = 0; i < dialog->num_choices; ++i) {
            Choice *choice = &dialog->choices[i];
            char *buf;
            asprintf(&buf, ">  %s", choice->text);
            draw_text_rec(buf,
                          rec,
                          dialog->font,
                          i == dialog->selected_choice_idx ? BLUE : BLACK);
            free(buf);
            // draw_text_rec(choice->text, rec, dialog->font);
            rec.y += 30;
        }
    }
}

void dialog_next_choice(Dialog *dialog) {
    dialog->selected_choice_idx++;
    if (dialog->selected_choice_idx >= dialog->num_choices)
        dialog->selected_choice_idx = dialog->num_choices - 1;
}

void dialog_prev_choice(Dialog *dialog) {
    dialog->selected_choice_idx--;
    if (dialog->selected_choice_idx < 0)
        dialog->selected_choice_idx = 0;
}

Choice *dialog_choose_selected_choice(Dialog *dialog) {
    return &dialog->choices[dialog->selected_choice_idx];
}

void var_set_int(DialogVar *var, int val) {
    sprintf(var->val_str, "%d", val);
}

void var_set_str(DialogVar *var, const char *val) {
    strcpy(var->val_str, val);
}

// returns true if there is a next dialog, false otherwise
bool conversation_continue(Conversation *convo) {
    const char *next_dialog_id;
    if (convo->curr_dialog->num_choices) {
        Choice *choice = dialog_choose_selected_choice(convo->curr_dialog);
        next_dialog_id = choice->next;
    } else
        next_dialog_id = convo->curr_dialog->next;

    if (!strcmp(next_dialog_id, "end")) {
        convo->curr_dialog = get_dialog_by_id(convo, "start");
        return false;
    }

    Dialog *next_dialog = get_dialog_by_id(convo, next_dialog_id);
    if (next_dialog) {
        convo->curr_dialog = next_dialog;
        return true;
    }
    fprintf(stderr, "Could not find specified next dialog '%s'\n", next_dialog_id);
    exit(EXIT_FAILURE);
}

void conversation_next_choice(Conversation *convo) {
    dialog_next_choice(convo->curr_dialog);
}

void conversation_prev_choice(Conversation *convo) {
    dialog_prev_choice(convo->curr_dialog);
}

void conversation_show(Conversation *convo, Rectangle rec) {
    dialog_show(convo->curr_dialog, rec);
}
