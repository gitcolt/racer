#ifndef DIALOG_H
#define DIALOG_H

#include "raylib.h"

#define MAX_LINES 3

typedef struct {
    const char *text_content;
    Font font;
} Dialog; 

void dialog_show(Dialog *dialog, Rectangle rec);

#endif
