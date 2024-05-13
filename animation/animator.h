#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "../spritesheet.h"

#define INITIAL_ANIM_SPEC_CAP 5

enum AnimMode {
    ANIM_MODE_ONE_SHOT,
    ANIM_MODE_LOOP,
    ANIM_MODE_STATIC,
};

typedef struct {
    int id;
    int frame_row;
    int nframes;
    int frame_duration;
    int curr_frame_idx;
    enum AnimMode mode;
} AnimSpec;

typedef struct {
    Spritesheet spritesheet;
    AnimSpec *anim_specs;
    AnimSpec *curr_anim_spec;
} Animator;

void animator_create(Animator *animator, Spritesheet spritesheet);
void animator_destroy(Animator *animator);
void animator_add_anim_spec(Animator *animator, AnimSpec anim_spec);
int animator_set_anim(Animator *animator, int anim_id);
void animator_set_anim_mode(Animator *animator, enum AnimMode mode);
void animator_update(Animator *animator, Tick tick);
void animator_draw(Animator *animator, Rectangle dst_rec);

#endif
