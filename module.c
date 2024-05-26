#include "hr.h"
#include "state/state.h"
#include "overworld/overworld.h"
#include "race/race.h"
#include "shared/dyn_arr.h"

#include "raylib.h"
#include "raylib-5.0/src/rlgl.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

void on_unload(void *ctx);
void on_load(void *ctx);

static void toggle_mode(State *state) {
    on_unload(state);

    state->game_mode = (state->game_mode + 1) > GAME_MODE_OVERWORLD
        ? 0
        : state->game_mode + 1;

    on_load(state);
}

static void process_common_input(State *state) {
    if (IsKeyPressed(KEY_M))
        toggle_mode(state);
}

void on_update(void *ctx) {
    State *state = ctx;

    process_common_input(state);

    switch (state->game_mode) {
        case GAME_MODE_EDITOR:
            track_editor_update(&state->editor_state);
            // state->editor_state.track.nodes[1];
            // ++state->editor_state.tick;
            break;
        case GAME_MODE_RACING:
            race_update(&state->race_state, state->race_state.tick);
            ++state->race_state.tick;
            break;
        case GAME_MODE_OVERWORLD:
            overworld_update(&state->overworld_state);
            ++state->overworld_state.tick;
            break;
    }
};

void on_load(void *ctx) {
    State *state = ctx;

    switch (state->game_mode) {
        case GAME_MODE_EDITOR:
            track_editor_load(&state->editor_state);
            break;
        case GAME_MODE_RACING:
            race_load(&state->race_state);
            break;
        case GAME_MODE_OVERWORLD:
            overworld_load(&state->overworld_state);
            break;
    }
};

void on_unload(void *ctx) {
    State *state = ctx;

    switch (state->game_mode) {
        case GAME_MODE_EDITOR:
            track_editor_unload(&state->editor_state);
            break;
        case GAME_MODE_RACING:
            // race_unload(&state->race_state);
            break;
        case GAME_MODE_OVERWORLD:
            overworld_unload(&state->overworld_state);
            break;
    }
}

void on_close(void *ctx) {
}

HR_HOOK_UPDATE(on_update);
HR_HOOK_LOAD(on_load);
HR_HOOK_UNLOAD(on_unload);
HR_HOOK_CLOSE(on_close);
