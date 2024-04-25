#ifndef TILES_H
#define TILES_H

#include "shared.h"

#include "raylib.h"
#include "cLDtk.h"

#define DST_TILE_SIZE 32

typedef struct {
    struct gridTiles *tiles;
    int src_tile_sz;
    Texture2D tilemap_tex;
} TileData;

void load_tiles();
void free_tiles();
void draw_tiles(TileData *tile_data, V2 offset);
void draw_grid(V2 offset);

#endif
