#ifndef DIALOG_H
#define DIALOG_H

#include "raylib.h"

#include <stddef.h>

#define MAX_CONVERSATION_DIALOGS_SZ 10
#define MAX_NODE_NAME_SZ 99
#define MAX_NODE_CONTENT_SZ 140
#define MAX_CHOICES_SZ 99
#define MAX_VARS_SZ 5

typedef struct {
    char text[MAX_NODE_CONTENT_SZ];
    char next[MAX_NODE_CONTENT_SZ];
} Choice;

typedef struct Dialog {
    Font font;
    char id[MAX_NODE_CONTENT_SZ];
    char text[MAX_NODE_CONTENT_SZ];
    union {
        char next[MAX_NODE_CONTENT_SZ];
        Choice choices[MAX_CHOICES_SZ];
    };
    size_t num_choices;
    char action[MAX_NODE_CONTENT_SZ];
} Dialog;

enum VarType {
    VAR_TYPE_NUM,
    VAR_TYPE_BOOL,
};

typedef struct {
    void *val;
    enum VarType type;
} Var;

typedef struct {
    char id[MAX_NODE_CONTENT_SZ];
    size_t num_dialogs;
    Dialog dialogs[MAX_CONVERSATION_DIALOGS_SZ];
    Var var[MAX_VARS_SZ];
} Conversation;

void dialog_init(Dialog *dialog);
void dialog_show(Dialog *dialog, Rectangle rectangle);
void var_set(Var *var, void *val, enum VarType type);

#endif
