#ifndef STATE_H
#define STATE_H

#include "../game_mode.h"
#include "../overworld/overworld_state.h"
#include "../race/race_state.h"
#include "../track_editor/track_editor.h"

typedef struct {
    enum GameMode game_mode;
    union {
        OverworldState overworld_state;
        TrackEditorState editor_state;
        RaceState race_state;
    };
} State;

#endif
