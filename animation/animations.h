#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "../spritesheet.h"
#include "animator.h"

void create_overworld_player_animator(Animator *animator, Spritesheet *spritesheet);
void create_overworld_npc_animator(Animator *animator, Spritesheet *spritesheet);

#endif
