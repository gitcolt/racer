#ifndef SHARED_H
#define SHARED_H

#include "raylib.h"

// TODO move some of this out

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WIN_POS_X 3000
#define WIN_POS_Y 0

#define ARRLEN(arr) \
    (sizeof(arr) / sizeof(*(arr)))

typedef Vector2 V2;
typedef long long Tick;

void draw_circle_lines_thickness(V2 center, float radius, float thickness, Color color);

enum AnimIdOverworldCharacter {
    ANIM_ID_WALK_UP,
    ANIM_ID_WALK_DOWN,
    ANIM_ID_WALK_LEFT,
    ANIM_ID_WALK_RIGHT,
};

char *ltrim(char *str);
char *rtrim(char *str);
char *trim(char *str);

typedef struct xml_node XmlNode;
typedef struct xml_string XmlString;
typedef struct xml_document XmlDocument;

#define ZERO_MEM(ptr) memset(ptr, 0, sizeof(*(ptr)))

#endif
