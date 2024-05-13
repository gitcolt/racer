#ifndef KART_H
#define KART_H

#include "shared.h"
#include "animation/animator.h"

typedef struct {
    Animator animator;
    V2 pos;
} Kart;

enum AnimId {
    ANIM_ID_TURN_LEFT,
    ANIM_ID_TURN_RIGHT,
};

void kart_init(Kart *kart);
void kart_draw(Kart *kart);
void kart_update(Kart *kart, Tick tick);

#endif
