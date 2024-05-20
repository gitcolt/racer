#define HR_DRIVER
#include "hr.h"
#include "colors.h"
#include "state/state.h"

#include "raylib.h"

#define INITIAL_GAME_MODE GAME_MODE_OVERWORLD

int main() {
    State state = {
        .game_mode = INITIAL_GAME_MODE,
    };
    // track_init(&state.editor_state.track);
    HR_CTX(state);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, ":)");
    SetWindowPosition(3000, 0);
    hr_init(".", "libmodule.so");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        DrawPoint3D((Vector3){0, 0, 0}, RED);
        ClearBackground(COLOR_OOB);
        hr_update();
        EndDrawing();
    }

    CloseWindow();
    hr_close();
}
