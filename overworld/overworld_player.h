#ifndef OVERWORLD_PLAYER_H
#define OVERWORLD_PLAYER_H

#include "../shared.h"
#include "../spritesheet.h"
#include "../animator.h"
#include "../collision.h"

#define PLAYER_COLLISION_RADIUS 10

enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
};

typedef struct {
    V2 pos;
    V2 dir;
    int walk_speed;
    Spritesheet spritesheet;
    Animator animator;
    CircleCollider collider;
} OverworldPlayer;

void overworld_player_update(OverworldPlayer *player, Tick tick);
void overworld_player_load(OverworldPlayer *player);
void overworld_player_unload(OverworldPlayer *player);
void overworld_player_draw(OverworldPlayer *player, V2 offset);
void overworld_player_set_dir(OverworldPlayer *player, enum Direction dir);
void overworld_player_move(OverworldPlayer *player);
void overworld_player_stop_moving(OverworldPlayer *player);

#endif
