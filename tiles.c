#include "raylib.h"
#include "tiles.h"
#include "shared.h"

#include <math.h>
#include <stdio.h>

void draw_tile(Rectangle src_rec,
               Rectangle dst_rec,
               Texture2D tex,
               V2 offset) {
    DrawTexturePro(tex,
                   src_rec,
                   dst_rec,
                   (V2){0,0},
                   0,
                   RAYWHITE);
}

void draw_tiles(TileData *tile_data, V2 offset) {
    for (int i = 0; i < tile_data->tiles->count; ++i) {
        struct gridTiles *tile = &tile_data->tiles[i];
        float x = (float)tile->x / tile_data->src_tile_sz;
        float y = (float)tile->y / tile_data->src_tile_sz;
        Rectangle src_rec = {
            tile->SRCx,
            tile->SRCy,
            tile_data->src_tile_sz,
            tile_data->src_tile_sz,
        };
        Rectangle dst_rec = {
            x * DST_TILE_SIZE + offset.x,
            y * DST_TILE_SIZE + offset.y,
            DST_TILE_SIZE,
            DST_TILE_SIZE,
        };
        draw_tile(src_rec, dst_rec, tile_data->tilemap_tex, offset);
    }
}

void load_tiles() {
    const char *map_data_filename = "LDtk_files/overworld.ldtk";
    loadJSONFile("{\"jsonVersion\":\"\"}", (char *)map_data_filename);
    importMapData();
}

void free_tiles() {
    freeMapData();
}

void draw_grid(V2 offset) {
    float thickness = 1;
    V2 start;
    V2 end;
    Color grid_color = BLACK;
    float offset_x = (int)offset.x % DST_TILE_SIZE - DST_TILE_SIZE;
    float offset_y = (int)offset.y % DST_TILE_SIZE - DST_TILE_SIZE;

    for (int col = 0; col < SCREEN_WIDTH / DST_TILE_SIZE + 3; ++col) {
        start = (V2){
            col * DST_TILE_SIZE + offset_x,
            0 + offset_y,
        };
        end = (V2){
            col * DST_TILE_SIZE + offset_x,
            SCREEN_HEIGHT + offset_y + 3 * DST_TILE_SIZE,
        };
        DrawLineEx(start, end, thickness, grid_color);
    }
    for (int row = 0; row < SCREEN_HEIGHT / DST_TILE_SIZE + 3; ++row) {
        start = (V2){
            0 + offset_x,
            row * DST_TILE_SIZE + offset_y,
        };
        end = (V2){
            SCREEN_WIDTH + offset_x + 3 * DST_TILE_SIZE,
            row * DST_TILE_SIZE + offset_y,
        };
        DrawLineEx(start, end, thickness, grid_color);
    }
}
