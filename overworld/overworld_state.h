#ifndef OVERWORLD_STATE_H
#define OVERWORLD_STATE_H

#include "overworld_player.h"
#include "npc.h"
#include "../spritesheet.h"
#include "../tiles.h"
#include "../collision.h"

typedef struct {
    V2 mouse_pos;
    Tick tick;
    Spritesheet tilemap;
    struct levels *lvl_one;
    TileData bg_tile_data;
    TileData mg_tile_data;
    TileData fg_tile_data;
    OverworldPlayer player;
    BoxCollider *colliders;
    NPC *npcs;
    size_t npcs_sz;
} OverworldState;

#endif
