#include "overworld_player.h"
#include "../tiles.h"
#include "../animation/animations.h"
#include "../shared/mymath.h"

#include <stdio.h>
#include <string.h>

void overworld_player_update(OverworldPlayer *player, Tick tick) {
    animator_update(&player->animator, tick);
}

void overworld_player_load(OverworldPlayer *player) {
    player->pos.x = 300;
    player->pos.y = 300;
    player->collider.center = player->pos;
    player->collider.radius = PLAYER_COLLISION_RADIUS;
    player->walk_speed = 3;
    player->dir.x = 0;
    player->dir.y = 0;
    create_overworld_player_animator(&player->animator, &player->spritesheet);
}

void overworld_player_unload(OverworldPlayer *player) {
    spritesheet_unload(&player->spritesheet);

    animator_destroy(&player->animator);
}

void overworld_player_draw(OverworldPlayer *player, V2 offset) {
    Rectangle dst_rec = {
        player->pos.x + offset.x - (float)DST_TILE_SIZE/2,
        player->pos.y + offset.y - (float)DST_TILE_SIZE/2,
        DST_TILE_SIZE,
        DST_TILE_SIZE,
    };
    animator_draw(&player->animator, dst_rec);

    draw_circle_collider(&player->collider, offset);

    char buf[99];
    sprintf(buf, "player pos: (%.1f, %.1f)", player->pos.x, player->pos.y);
    DrawText(buf, 10, 10, 20, BLACK);

    // draw direction vector
    V2 a = v2_add(player->pos, offset);
    V2 b = v2_add(v2_add(player->pos, v2_mul_scalar(player->dir, 20)), offset);
    DrawLineEx(a, b, 3, RED);
}

void overworld_player_stop_moving(OverworldPlayer *player) {
    player->dir.x = 0;
    player->dir.y = 0;
    player->animator.curr_anim_spec->curr_frame_idx = 0;
    animator_set_anim_mode(&player->animator, ANIM_MODE_STATIC);
}

void overworld_player_set_dir(OverworldPlayer *player, enum Direction dir) {
    animator_set_anim_mode(&player->animator, ANIM_MODE_LOOP);

    switch (dir) {
        case DIR_UP:
            player->dir.y -= 1;
            animator_set_anim(&player->animator, ANIM_ID_WALK_UP);
            break;
        case DIR_DOWN:
            player->dir.y += 1;
            animator_set_anim(&player->animator, ANIM_ID_WALK_DOWN);
            break;
        case DIR_LEFT:
            player->dir.x -= 1;
            animator_set_anim(&player->animator, ANIM_ID_WALK_LEFT);
            break;
        case DIR_RIGHT:
            player->dir.x += 1;
            animator_set_anim(&player->animator, ANIM_ID_WALK_RIGHT);
            break;
    }
    player->collider.center = player->pos;
}

void overworld_player_move(OverworldPlayer *player) {
    player->dir = v2_normalize(player->dir);
    player->pos = v2_add(player->pos, v2_mul_scalar(player->dir, player->walk_speed));

}
