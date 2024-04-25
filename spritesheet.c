#include "spritesheet.h"

#include "raylib.h"

void spritesheet_load(Spritesheet *spritesheet,
                      const char *filename,
                      int ncols,
                      int nrows) {
    spritesheet->ncols = ncols;
    spritesheet->nrows = nrows;
    spritesheet->tex = LoadTexture(filename);
}

void spritesheet_unload(Spritesheet *spritesheet) {
    UnloadTexture(spritesheet->tex);
}

void spritesheet_draw_frame(Spritesheet *spritesheet,
                            int col,
                            int row,
                            Rectangle dst_rec) {
    Texture2D tex = spritesheet->tex;
    int frame_width = tex.width / spritesheet->ncols;
    int frame_height = tex.height / spritesheet->nrows;
    Rectangle src_rec = {
        .x = col * frame_width,
        .y = row * frame_height,
        .width = frame_width,
        .height = frame_height,
    };
    DrawTexturePro(tex, src_rec, dst_rec, (Vector2){0,0}, 0, RAYWHITE);
}
