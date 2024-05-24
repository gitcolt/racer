#ifndef RACE_H
#define RACE_H

#include "race_state.h"

void race_update(RaceState *state, Tick tick);
void race_load(RaceState *state);

#endif
