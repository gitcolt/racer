#ifndef DIALOG_H
#define DIALOG_H

#include "raylib.h"

#include <stddef.h>

#define MAX_LINES 3

#define MAX_CONVERSATION_DIALOGS_SZ 10
#define MAX_NODE_NAME_SZ 99
#define MAX_NODE_CONTENT_SZ 999
#define MAX_CHOICES_SZ 99

typedef struct {
    char text[MAX_NODE_CONTENT_SZ];
    char next[MAX_NODE_CONTENT_SZ];
} Choice;

typedef struct Dialog {
    Font font;
    char id[MAX_NODE_CONTENT_SZ];
    char text[MAX_NODE_CONTENT_SZ];
    Choice choices[MAX_CHOICES_SZ];
    size_t choices_sz;
    char next[MAX_NODE_CONTENT_SZ];
    char action[MAX_NODE_CONTENT_SZ];
} Dialog;

typedef struct {
    char id[MAX_NODE_CONTENT_SZ];
    size_t num_dialogs;
    Dialog dialogs[MAX_CONVERSATION_DIALOGS_SZ];
} Conversation;

void dialog_show(Dialog *dialog, Rectangle rec);

#endif
