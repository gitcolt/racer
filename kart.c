#include "kart.h"

#include <malloc.h>

static void initialize_animations(Kart *kart, Spritesheet spritesheet) {
    animator_create(&kart->animator, spritesheet);

    animator_add_anim_spec(&kart->animator, (AnimSpec){ ANIM_ID_TURN_LEFT, 0, 17, 5, 0 });
    // animator_add_anim_spec(&kart->animator, (FrameSpec){ ANIM_ID_TURN_RIGHT, 1, 17, 5, 0 });

    animator_set_anim(&kart->animator, ANIM_ID_TURN_LEFT);
}

void kart_init(Kart *kart) {
    kart->pos.x = 0;
    kart->pos.y = 0;
    Spritesheet spritesheet = {};
    spritesheet_load(&spritesheet, "./assets/yoshi_kart_17x1.png", 17, 1);

    initialize_animations(kart, spritesheet);
}

void kart_destroy(Kart *kart) {
    animator_destroy(&kart->animator);
}

void kart_draw(Kart *kart) {
    Rectangle dst_rec = {
        .x = kart->pos.x,
        .y = kart->pos.y,
        .width = 150,
        .height = 150,
    };
    animator_draw(&kart->animator, dst_rec);
}

void kart_update(Kart *kart, Tick tick) {
    animator_update(&kart->animator, tick);
}
