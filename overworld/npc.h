#ifndef NPC_H
#define NPC_H

#include "../dialog/dialog.h"
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
    Conversation *convo;
} NPC;

void npc_load(NPC *npc, V2 tile_pos, const char *identifier, Conversation *convo);
void npc_unload(NPC *npc);
void npc_update(NPC *npc, Tick tick);
void npc_draw(NPC *npc, V2 offset);

#endif
