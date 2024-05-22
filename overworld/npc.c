#include "npc.h"
#include "../dyn_arr.h"
#include "../mymath.h"
#include "../tiles.h"
#include "../animation/animations.h"

#include <malloc.h>
#include <string.h>

void npc_draw(NPC *npc, V2 offset) {
    Rectangle rec = {
        .x = npc->pos.x + offset.x,
        .y = npc->pos.y + offset.y,
        .width = DST_TILE_SIZE,
        .height = DST_TILE_SIZE,
    };
    animator_draw(&npc->animator, rec);
    draw_circle_collider(&npc->collider, offset);
}

void npc_update(NPC *npc, Tick tick) {
    animator_update(&npc->animator, tick);
}

void npc_load(NPC *npc, V2 tile_pos, const char *identifier, Conversation *convo) {
    npc->name = identifier;
    npc->pos = v2_mul_scalar(tile_pos, DST_TILE_SIZE);
    npc->dir = (V2){0, 0};
    npc->collider = (CircleCollider){
        .center = v2_add(npc->pos, (V2){(float)DST_TILE_SIZE/2,(float)DST_TILE_SIZE/2}),
        .radius = NPC_COLLISION_RADIUS,
    };
    create_overworld_npc_animator(&npc->animator, &npc->spritesheet);
    npc->convo = convo;
}

void npc_unload(NPC *npc) {
    animator_destroy(&npc->animator);
}
