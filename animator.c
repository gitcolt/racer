#include "animator.h"
#include "spritesheet.h"
#include "dyn_arr.h"
#include "debug.h"

#include <stdio.h>

static AnimSpec *get_anim_spec(Animator *animator, int anim_id) {
    for (int i = 0; i < ARR_LEN(animator->anim_specs); ++i) {
        AnimSpec *anim_spec = &animator->anim_specs[i];
        if (anim_spec->id == anim_id)
            return anim_spec;
    }
    return NULL;
}

void animator_create(Animator *animator, Spritesheet spritesheet) {
    animator->anim_specs = ARR(AnimSpec, INITIAL_ANIM_SPEC_CAP);
    animator->curr_anim_spec = NULL;
    animator->spritesheet = spritesheet;
}

void animator_destroy(Animator *animator) {
    arr_free(animator->anim_specs);
}

void animator_add_anim_spec(Animator *animator, AnimSpec anim_spec) {
    ARR_APPEND(animator->anim_specs, anim_spec);
}

int animator_set_anim(Animator *animator, int anim_id) {
    AnimSpec *anim_spec = get_anim_spec(animator, anim_id);
    if (!anim_spec)
        return 1;

    animator->curr_anim_spec = anim_spec;
    return 0;
}

void animator_set_anim_mode(Animator *animator, enum AnimMode mode) {
    animator->curr_anim_spec->mode = mode;
};

void animator_update(Animator *animator, Tick tick) {
    if (animator->curr_anim_spec->mode == ANIM_MODE_STATIC)
        return;
    else if (animator->curr_anim_spec->mode == ANIM_MODE_LOOP) {
        AnimSpec *curr_anim_spec = animator->curr_anim_spec;
        if (!curr_anim_spec)
            ERR_EXIT("No animation has been set\n");

        if (curr_anim_spec->frame_duration == 0 || curr_anim_spec->nframes == 0)
            return;
        curr_anim_spec->curr_frame_idx =
            (tick / curr_anim_spec->frame_duration)  % curr_anim_spec->nframes;
    } else if (animator->curr_anim_spec->mode == ANIM_MODE_ONE_SHOT) {
        printf("TODO: ANIM_MODE_ONE_SHOT\n");
    }
}

void animator_draw(Animator *animator, Rectangle dst_rec) {
    Texture2D tex = animator->spritesheet.tex;
    int frame_width = tex.width / animator->spritesheet.ncols;
    int frame_height = tex.height / animator->spritesheet.nrows;

    AnimSpec *fs = animator->curr_anim_spec;

    spritesheet_draw_frame(&animator->spritesheet, fs->curr_frame_idx, fs->frame_row, dst_rec);
}
