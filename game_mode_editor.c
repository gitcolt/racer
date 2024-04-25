#include "game_mode_editor.h"
#include "state/state.h"

#include "shared.h"

static V2 mouse_pos = { 0, 0 };

void editor_on_load(State *state) {
}

void editor_on_update(State *state) {
}

// void editor_process_input(State *state) {
//     mouse_pos = GetMousePosition();
// 
//     if (state->editor_state.editing_mode == MODE_ADDING) {
//         if (IsKeyPressed(KEY_SPACE))
//             track_add_node_at(&state->editor_state.track, mouse_pos);
//         if (IsKeyPressed(KEY_C))
//             track_clear_nodes(&state->editor_state.track);
// 
//         // move node
//         if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//             grabbed_node_el = track_grab_node_el_at(&state->track, mouse_pos);
//             if (grabbed_node_el.type != NODE_EL_NONE)
//                 start_node_el_drag(grabbed_node_el);
//         }
//         if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
//             stop_node_el_drag();
//     }
// 
//     if (IsKeyPressed(KEY_M))
//         toggle_editing_mode(state);
// }
