#include "animations.h"

void create_overworld_player_animator(Animator *animator, Spritesheet *spritesheet) {
    spritesheet_load(spritesheet, "./assets/pokemon.png", 3, 12);
    animator_create(animator, *spritesheet);
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
    animator_add_anim_spec(animator, anim_spec_walk_up);
    animator_add_anim_spec(animator, anim_spec_walk_down);
    animator_add_anim_spec(animator, anim_spec_walk_left);
    animator_add_anim_spec(animator, anim_spec_walk_right);
    animator_set_anim(animator, ANIM_ID_WALK_DOWN);
}

void create_overworld_npc_animator(Animator *animator, Spritesheet *spritesheet) {
    spritesheet_load(spritesheet, "./assets/pokemon.png", 3, 12);
    animator_create(animator, *spritesheet);
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
        .mode = ANIM_MODE_LOOP,
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
    animator_add_anim_spec(animator, anim_spec_walk_up);
    animator_add_anim_spec(animator, anim_spec_walk_down);
    animator_add_anim_spec(animator, anim_spec_walk_left);
    animator_add_anim_spec(animator, anim_spec_walk_right);
    animator_set_anim(animator, ANIM_ID_WALK_DOWN);
    animator_set_anim_mode(animator, ANIM_MODE_LOOP);
}
