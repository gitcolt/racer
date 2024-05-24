#ifndef COLLISION_H
#define COLLISION_H

#include "shared/shared.h"

#include "raylib.h"

typedef struct {
    Rectangle rec;
} BoxCollider;

typedef struct {
    V2 center;
    float radius;
} CircleCollider;

void draw_box_collider(BoxCollider *collider, V2 offset);
void draw_circle_collider(CircleCollider *collider, V2 offset);

#endif
