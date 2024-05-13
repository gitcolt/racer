#ifndef NPC_H
#define NPC_H

#include "../collision.h"
#include "../spritesheet.h"
#include "../animation/animator.h"

#define NPC_COLLISION_RADIUS 20

typedef struct {
    const char *name;
    V2 pos;
    V2 dir;
    Spritesheet spritesheet;
    Animator animator;
    CircleCollider collider;
} NPC;

void npc_load(NPC *npc, V2 tile_pos, const char *identifier);
void npc_draw(NPC *npc, V2 offset);

#endif
