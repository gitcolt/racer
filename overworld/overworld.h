#ifndef OVERWORLD_H
#define OVERWORLD_H

#include "overworld_state.h"

void overworld_draw(OverworldState *state);
void overworld_process_input(OverworldState *state);
void overworld_update(OverworldState *state);
void overworld_load(OverworldState *state);
void overworld_unload(OverworldState *state);

#endif
