#include "track_editor.h"
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

#if 0
void process_input(State *state) {
    mouse_pos = GetMousePosition();

    if (state->editing_mode == MODE_ADDING) {
        if (IsKeyPressed(KEY_SPACE))
            track_add_node_at(&state->track, mouse_pos);
        if (IsKeyPressed(KEY_C))
            track_clear_nodes(&state->track);

        // move node
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            grabbed_node_el = track_grab_node_el_at(&state->track, mouse_pos);
            if (grabbed_node_el.type != NODE_EL_NONE)
                start_node_el_drag(grabbed_node_el);
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            stop_node_el_drag();
    }

    if (IsKeyPressed(KEY_M))
        toggle_editing_mode(state);
}
#endif

// void update_things(State *state) {
//     Node *first_node = &state->track.nodes[0];
//     Node *last_node  = &state->track.nodes[ARR_LEN(state->track.nodes) - 1];
//     if (grabbed_node_el.type != NODE_EL_NONE) {
//         switch (grabbed_node_el.type) {
//             case NODE_EL_NONE:
//                 break;
//             case NODE_EL_NODE:
//                 node_move(grabbed_node_el.node, mouse_pos);
//                 if (grabbed_node_el.node == last_node)
//                     node_move(first_node, mouse_pos);
//                 else if (grabbed_node_el.node == first_node)
//                     node_move(last_node, mouse_pos);
//                 break;
//             case NODE_EL_L_HANDLE:
//                 set_left_handle_pos(grabbed_node_el.node, mouse_pos);
//                 if (grabbed_node_el.node == last_node)
//                     set_left_handle_pos(first_node, mouse_pos);
//                 else if (grabbed_node_el.node == first_node)
//                     set_left_handle_pos(last_node, mouse_pos);
//                 break;
//             case NODE_EL_R_HANDLE:
//                 set_right_handle_pos(grabbed_node_el.node, mouse_pos);
//                 if (grabbed_node_el.node == last_node)
//                     set_right_handle_pos(first_node, mouse_pos);
//                 else if (grabbed_node_el.node == first_node)
//                     set_right_handle_pos(last_node, mouse_pos);
//                 break;
//         }
//     }
// }

void on_unload(void *ctx);
void on_load(void *ctx);

static void toggle_mode(State *state) {
    on_unload(state);
    state->game_mode = state->game_mode == GAME_MODE_OVERWORLD
        ? GAME_MODE_RACING
        : GAME_MODE_OVERWORLD;
    on_load(state);
}

static void process_common_input(State *state) {
    if (IsKeyPressed(KEY_M))
        toggle_mode(state);
}

#if 1
void on_update(void *ctx) {
    State *state = ctx;

    process_common_input(state);

    switch (state->game_mode) {
        case GAME_MODE_EDITOR:
            printf("on_update editor\n");
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
#endif

#if 1
void on_load(void *ctx) {
    State *state = ctx;

    switch (state->game_mode) {
        case GAME_MODE_EDITOR:
            printf("on_load editor\n");
            break;
        case GAME_MODE_RACING:
            race_load(&state->race_state);
            break;
        case GAME_MODE_OVERWORLD:
            overworld_load(&state->overworld_state);
            break;
    }
};
#endif

void on_unload(void *ctx) {
    State *state = ctx;

    switch (state->game_mode) {
        case GAME_MODE_EDITOR:
            printf("on_unload editor\n");
            break;
        case GAME_MODE_RACING:
            // race_unload(&state->race_state);
            break;
        case GAME_MODE_OVERWORLD:
            overworld_unload(&state->overworld_state);
            break;
    }
}

void on_close() {
}

HR_HOOK_UPDATE(on_update);
HR_HOOK_LOAD(on_load);
HR_HOOK_UNLOAD(on_unload);
HR_HOOK_CLOSE(on_close);
