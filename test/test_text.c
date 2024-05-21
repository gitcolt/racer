#include "../text.h"
#include "../shared.h"

#include "minunit.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define LAST_FUNC_NAME_LEN 1024
static char last_func_name[LAST_FUNC_NAME_LEN];
#define SET_LAST_FUNC_NAME() \
    strcpy(last_func_name, __func__)

#define DEFINE_TEXT_REC_TEST(id, text) \
    _DEFINE_TEXT_REC_TEST(test_text_rec_, text, id)

#define _DEFINE_TEXT_REC_TEST(test_name, text, id) \
    MU_TEST(test_name##id) { \
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, text[0] == '\0' ? "(empty)" : text); \
        SetWindowPosition(WIN_POS_X, WIN_POS_Y); \
        BeginDrawing();  \
        ClearBackground(ORANGE); \
        const char *text_to_draw = (text); \
        Rectangle rec = { \
            .x = 100, \
            .y = 100, \
            .width = 250, \
            .height = 300, \
        }; \
        Font font = GetFontDefault(); \
        DrawRectangleRec(rec, BLUE); \
        draw_text_rec(text_to_draw, rec, font); \
        SET_LAST_FUNC_NAME(); \
    }

DEFINE_TEXT_REC_TEST(testing123, "This is some test text. Testing 1 2 3.")
DEFINE_TEXT_REC_TEST(smol, "smol")
DEFINE_TEXT_REC_TEST(empty, "")
DEFINE_TEXT_REC_TEST(long, "Tell me, O Muse, of that ingenious hero who travelled far and wide after he had sacked the famous town of Troy. Many cities did he visit, and many were the nations with whose manners and customs he was acquainted; moreover he suffered much by the sea while trying to save his own life and bring his men safely home; but do what he might he could not save his men, for they perished through their own sheer folly in eating the cattle of the Sun and Hyperion; so the god prevented them from ever reaching home. Tell me, too, about all these things, oh daughter of Jove, from whatsoever source you may know them.")
DEFINE_TEXT_REC_TEST(newline, "There should be a line break right here\n and also here\n and here.\n")

void (*tests[])(void) = {
    test_text_rec_testing123,
    test_text_rec_smol,
    test_text_rec_empty,
    test_text_rec_long,
    test_text_rec_newline,
};

MU_TEST_SUITE(test_suite) {
    SetTraceLogLevel(LOG_ERROR);
    for (int i = 0; i < ARRLEN(tests); ++i) {
        MU_RUN_TEST(tests[i]);
        EndDrawing();
        printf("Press 'f' to fail, [Enter] to pass\n");
        char ch = getchar();
        if (ch == 'f') {
            minunit_fail++;
            printf("%s failed\n", last_func_name);
            // flush input
            while ((ch = getchar()) != '\n');
        }
        CloseWindow();
    }
}

int main() {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
}
