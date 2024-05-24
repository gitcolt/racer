#include "race.h"
#include "../shared/shared.h"

#include "raylib.h"

#include <stdio.h>

static Camera3D cam = {
    .position = {5.f, 2.f, 5.f},
    .target = {0.0f, 0.0f, 0.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .fovy = 45.0f,
    .projection = CAMERA_PERSPECTIVE,
};

static Texture2D track_tex;

static void race_draw(RaceState *state) {
    kart_draw(&state->player_kart);
}

void race_update(RaceState *state, Tick tick) {
    kart_update(&state->player_kart, tick);
    UpdateCamera(&cam, CAMERA_FIRST_PERSON);


    race_draw(state);
    BeginMode3D(cam);
    draw_textured_quad(track_tex, (V2){0, 0});
    EndMode3D();

}

void race_load(RaceState *state) {
    printf("race_load()\n");
    kart_init(&state->player_kart);

    const char *track_tex_filename = "./assets/track.png";
    track_tex = LoadTexture(track_tex_filename);
}
