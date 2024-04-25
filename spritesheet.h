#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "shared.h"

#include "raylib.h"

typedef struct {
    Texture2D tex;
    int ncols;
    int nrows;
} Spritesheet;

void spritesheet_load(Spritesheet *spritesheet,
                      const char *filename,
                      int ncols,
                      int nrows);

void spritesheet_unload(Spritesheet *spritesheet);

void spritesheet_draw_frame(Spritesheet *spritesheet,
                            int col,
                            int row,
                            Rectangle dst_rec);

#endif
