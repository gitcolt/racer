#include "collision.h"
#include "shared.h"
#include "colors.h"
#include "mymath.h"

static const float box_collider_thickness = 3;
static const float circle_collider_thickness = 3;

void draw_box_collider(BoxCollider *collider, V2 offset) {
    Rectangle rec = {
        .x = collider->rec.x + offset.x,
        .y = collider->rec.y + offset.y,
        .width = collider->rec.width,
        .height = collider->rec.height,
    };
    DrawRectangleLinesEx(collider->rec, box_collider_thickness, COLOR_COLLIDER);
}

void draw_circle_collider(CircleCollider *collider, V2 offset) {
    draw_circle_lines_thickness(v2_add(collider->center, offset),
                                collider->radius,
                                circle_collider_thickness,
                                COLOR_COLLIDER);
}
