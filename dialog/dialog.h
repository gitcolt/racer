#ifndef DIALOG_H
#define DIALOG_H

#include "raylib.h"

#include <stddef.h>

#define MAX_CONVERSATION_DIALOGS_SZ 10
#define MAX_NODE_NAME_SZ 99
#define MAX_NODE_CONTENT_SZ 140
#define MAX_CHOICES_SZ 99
#define MAX_VARS_SZ 5
#define MAX_VAR_VAL_STR 32
#define MAX_VAR_ID 32

typedef struct {
    char text[MAX_NODE_CONTENT_SZ];
    char next[MAX_NODE_CONTENT_SZ];
} Choice;

typedef struct Dialog {
    Font font;
    char id[MAX_NODE_CONTENT_SZ];
    char text[MAX_NODE_CONTENT_SZ];
    char next[MAX_NODE_CONTENT_SZ];
    Choice choices[MAX_CHOICES_SZ];
    size_t num_choices;
    char action[MAX_NODE_CONTENT_SZ];
} Dialog;

// TODO: Get rid of this enum
enum VarType {
    VAR_TYPE_NUM,
    VAR_TYPE_STR,
};

typedef struct {
    char id[MAX_VAR_ID];
    char val_str[MAX_VAR_VAL_STR];
    enum VarType type;
} DialogVar;

typedef struct {
    char id[MAX_NODE_CONTENT_SZ];
    size_t num_dialogs;
    Dialog dialogs[MAX_CONVERSATION_DIALOGS_SZ];
    DialogVar vars[MAX_VARS_SZ];
    size_t num_vars;
} Conversation;

void dialog_init(Dialog *dialog);
void dialog_show(Dialog *dialog, Rectangle rectangle);
void var_set_int(DialogVar *var, int val);
void var_set_str(DialogVar *var, const char *val);

#endif
