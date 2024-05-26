#define HR_DRIVER
#include "hr.h"
#include "shared/colors.h"
#include "state/state.h"

#include "raylib.h"

#define INITIAL_GAME_MODE GAME_MODE_EDITOR

int main() {
    State state = {
        .game_mode = INITIAL_GAME_MODE,
    };
    HR_CTX(state);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, ":)");
    SetWindowPosition(WIN_POS_X, WIN_POS_Y);
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
