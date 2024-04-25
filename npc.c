#include "npc.h"
#include "dyn_arr.h"
#include "mymath.h"
#include "tiles.h"

#include <malloc.h>
#include <string.h>

void npc_draw(NPC *npc, V2 offset) {
    Rectangle rec = {
        .x = npc->pos.x + offset.x,
        .y = npc->pos.y + offset.y,
        .width = DST_TILE_SIZE,
        .height = DST_TILE_SIZE,
    };
    spritesheet_draw_frame(&npc->spritesheet, 0, 1, rec);
}

void npc_load(NPC *npc, V2 tile_pos, const char *identifier) {
    npc->name = identifier;
    npc->pos = v2_mul_scalar(tile_pos, DST_TILE_SIZE);
    npc->dir = (V2){0, 0};
    spritesheet_load(&npc->spritesheet, "./assets/pokemon.png", 3, 12);
    npc->collider = (CircleCollider){
        .center = npc->pos,
        .radius = NPC_COLLISION_RADIUS,
    };

    animator_create(&npc->animator, npc->spritesheet);
    AnimSpec anim_spec_walk_up = {
        .id = ANIM_ID_WALK_UP,
        .mode = ANIM_MODE_STATIC,
        .nframes = 3,
        .frame_row = 0,
        .curr_frame_idx = 0,
        .frame_duration = 10,
    };
    AnimSpec anim_spec_walk_down = {
        .id = ANIM_ID_WALK_DOWN,
        .mode = ANIM_MODE_STATIC,
        .nframes = 3,
        .frame_row = 1,
        .curr_frame_idx = 0,
        .frame_duration = 10,
    };
    AnimSpec anim_spec_walk_left = {
        .id = ANIM_ID_WALK_LEFT,
        .mode = ANIM_MODE_STATIC,
        .nframes = 2,
        .frame_row = 2,
        .curr_frame_idx = 0,
        .frame_duration = 10,
    };
    AnimSpec anim_spec_walk_right = {
        .id = ANIM_ID_WALK_RIGHT,
        .mode = ANIM_MODE_STATIC,
        .nframes = 2,
        .frame_row = 3,
        .curr_frame_idx = 0,
        .frame_duration = 10,
    };
    animator_add_anim_spec(&npc->animator, anim_spec_walk_up);
    animator_add_anim_spec(&npc->animator, anim_spec_walk_down);
    animator_add_anim_spec(&npc->animator, anim_spec_walk_left);
    animator_add_anim_spec(&npc->animator, anim_spec_walk_right);
    animator_set_anim(&npc->animator, ANIM_ID_WALK_DOWN);
}
