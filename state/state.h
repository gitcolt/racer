#ifndef STATE_H
#define STATE_H

#include "editor_state.h"
#include "../game_mode.h"
#include "../overworld/overworld_state.h"
#include "../race/race_state.h"

typedef struct {
    enum GameMode game_mode;
    union {
        OverworldState overworld_state;
        EditorState editor_state;
        RaceState race_state;
    };
} State;

#endif
