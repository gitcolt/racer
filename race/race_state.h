#ifndef RACING_STATE_H
#define RACING_STATE_H

#include "../kart.h"

typedef struct {
    Tick tick;
    Kart player_kart;
} RaceState;

#endif
