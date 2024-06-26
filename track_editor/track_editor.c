#include "track_editor.h"
#include "raylib.h"
#define DYN_ARR_IMPL
#include "../shared/dyn_arr.h"
#include "../shared/mymath.h"

#include "rlgl.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <string.h>

static V2 mouse_pos = {0};

typedef struct {
    enum NodeElementType type;
    Node *node;
} GrabbedNode;

static GrabbedNode grabbed_node = { .type = NODE_EL_NONE };
static bool is_texture_loaded = false;
static RenderTexture2D render_tex;
static float track_geo_spacing = 20;

void track_clear_nodes(Track *track) {
    arr_clear(track->nodes);
    arr_clear(track->verts);
}

void track_gen_geo(Track *track) {
    Node *nodes = track->nodes;
    size_t num_nodes = ARR_LEN(nodes);
    if (num_nodes < 2)
        return;

    if (!is_texture_loaded) {
        render_tex = LoadRenderTexture(800, 600);
        is_texture_loaded = true;
    }

    ARR_LEN(track->verts) = 0;

    for (size_t i = 0; i < num_nodes; ++i) {
        Node *n  = &nodes[i    ];
        int next_i = i + 1;
        if (next_i == num_nodes) {
            if (!track->closed)
                break;
            next_i = 0;
        }
        Node *nn = &nodes[next_i];
        V2 p1 = n->pos;
        V2 c2 = n->handles[1];
        V2 c3 = nn->handles[0];
        V2 p4 = nn->pos;
        float t = 0;
        float delta_t = .1;

        while (t <= 1) {
            V2 a = GetSplinePointBezierCubic(p1, c2, c3, p4, t);
            DrawCircleV(a, 5, MAROON);
            t += delta_t;
            V2 b = GetSplinePointBezierCubic(p1, c2, c3, p4, t);

            float d = v2_dist(a, b);
            d /= track_geo_spacing;

            V2 dir = v2_sub(b, a);
            dir = v2_normalize(dir);

            static float track_width = 20;
            V2 transverse_dir_l = v2_mul_scalar(dir, track_width);
            transverse_dir_l = v2_rot_90_ccw(transverse_dir_l, (V2){0, 0});
            V2 transverse_dir_r = v2_rot_180(transverse_dir_l, (V2){0, 0});

            float t2 = 0;
            while (t2 < 1) {
                V2 p = GetSplinePointLinear(a, b, t2);
                V2 transverse_point_l = v2_add(p, transverse_dir_l);
                V2 transverse_point_r = v2_add(p, transverse_dir_r);
                ARR_APPEND(track->verts, transverse_point_l);
                ARR_APPEND(track->verts, transverse_point_r);

                DrawCircleV(transverse_point_l, 3, LIME);
                DrawCircleV(transverse_point_r, 3, LIME);
                DrawCircleV(p, 3, BLUE);
                t2 += 1/d;
            }
        }
    }
    if (track->closed) {
        ARR_APPEND(track->verts, track->verts[0]);
        ARR_APPEND(track->verts, track->verts[1]);
    }
}

void track_draw_geo(Track *track) {
    size_t num_verts = ARR_LEN(track->verts);
    if (num_verts == 0)
        return;

    size_t len = ARR_LEN(track->verts);
    size_t cap = ARR_CAPACITY(track->verts);
    ArrHeader *h = ARR_HEADER(track->verts);
    if (len > 0) {
        // printf("ARR_LEN(): %zu\n", len);
        // printf("ARR_CAPACITY(): %zu\n", cap);
    }

    BeginTextureMode(render_tex);
    ClearBackground(RAYWHITE);

    DrawTriangleStrip(track->verts, num_verts, LIGHTGRAY);
    for (size_t i = 0; i < num_verts - 1; ++i) {
        V2 va = track->verts[i    ];
        V2 vb = track->verts[i + 1];
        DrawLineV(va, vb, BLACK);
    }

    EndTextureMode();

    DrawTextureRec(render_tex.texture,
                   (Rectangle){
                        0,
                        0,
                        render_tex.texture.width,
                        -render_tex.texture.height},
                   (V2){0, 0},
                   WHITE);
    Rectangle src_rec = {
        .x = 0, .y = 0,
        render_tex.texture.width, -render_tex.texture.height,
    };
    Rectangle dst_rec = {
        .x = 0, .y = 0,
        100, 100,
    };
    DrawTexturePro(render_tex.texture,
            src_rec,
            dst_rec,
                   (V2){0, 0},
                   0,
                   RAYWHITE);
}

void node_move_group(Node *node, V2 pos) {
    V2 delta = v2_sub(pos, node->pos);
    node->pos = pos;
    node->handles[0] = v2_add(node->handles[0], delta);
    node->handles[1] = v2_add(node->handles[1], delta);
}

void node_move_l_handle(Node *node, V2 pos) {
    node->handles[0] = pos;
    node->handles[1] = v2_rot_180(node->handles[0], node->pos);
}

void node_move_r_handle(Node *node, V2 pos) {
    node->handles[1] = pos;
    node->handles[0] = v2_rot_180(node->handles[1], node->pos);
}

void node_move(Node *node, enum NodeElementType type, V2 pos) {
    switch (type) {
        case NODE_EL_L_HANDLE:
            node_move_l_handle(node, pos);
            break;
        case NODE_EL_R_HANDLE:
            node_move_r_handle(node, pos);
            break;
        case NODE_EL_NODE:
            node_move_group(node, pos);
            break;
        case NODE_EL_NONE:
            break;
    }
}

void track_add_node_at(Track *track, V2 pos) {
    Node node = {
        .pos = pos,
        .handles[0] = pos,
        .handles[1] = pos,
    };

    Node *first_node  = &track->nodes[0];
    float d = v2_dist(pos, first_node->pos);
    float snap_threshold_dist = 10;
    if (ARR_LEN(track->nodes) > 2 && d < snap_threshold_dist) {
        // printf("SNAPPING\n");
        // ARR_APPEND(track->nodes, *first_node);
        track->closed = true;
    }
    else
        ARR_APPEND(track->nodes, node);
}

void draw_node_handle(V2 pos) {
    static int handle_radius = 7;
    DrawCircle(pos.x, pos.y, handle_radius, BLUE);
}

void draw_node_node(V2 pos) {
    static int node_radius = 10;
    DrawCircle(pos.x, pos.y, node_radius, RED);
}

void node_draw_handle_lines(Node *node) {
    static float handle_line_thickness = 3;
    DrawLineEx(node->pos, node->handles[0], handle_line_thickness, BLACK);
    DrawLineEx(node->pos, node->handles[1], handle_line_thickness, BLACK);
}

void node_draw(Node *node) {
    node_draw_handle_lines(node);
    draw_node_node(node->pos);
    draw_node_handle(node->handles[0]);
    draw_node_handle(node->handles[1]);
}

void track_draw(Track *track) {
    size_t num_nodes = ARR_LEN(track->nodes);
    if (num_nodes == 0)
        return;

    for (size_t i = 0; i < num_nodes; ++i) {
        V2 p1 = track->nodes[i].pos;
        V2 c2 = track->nodes[i].handles[1];
        int next_i = i + 1;
        if (next_i == num_nodes) {
            if (!track->closed) {
                node_draw(&track->nodes[i]);
                break;
            }
            next_i = 0;
        }
        V2 c3 = track->nodes[next_i].handles[0];
        V2 p4 = track->nodes[next_i].pos;
        float thickness = 4;
        DrawSplineSegmentBezierCubic(p1, c2, c3, p4, thickness, ORANGE);
        node_draw(&track->nodes[i]);
    }
    node_draw(&track->nodes[0]);
}

void editor_process_input(TrackEditorState *state) {
    mouse_pos = GetMousePosition();
    char buf[99];
    sprintf(buf, "# nodes: %zu, mouse_pos: (%.1f, %.1f)", ARR_LEN(state->track.nodes), mouse_pos.x, mouse_pos.y);
    DrawText(buf, 10, 10, 25, WHITE);

    Track *track = &state->track;

    if (IsKeyPressed(KEY_SPACE)) {
        track_add_node_at(track, mouse_pos);
    }

    if (IsKeyPressed(KEY_C)) {
        track_clear_nodes(&state->track);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < ARR_LEN(track->nodes); ++i) {
            Node *n = &track->nodes[i];
            if (CheckCollisionPointCircle(mouse_pos, n->handles[0], 10)) {
                grabbed_node.type = NODE_EL_L_HANDLE;
                grabbed_node.node = n;
                break;
            }
            if (CheckCollisionPointCircle(mouse_pos, n->handles[1], 10)) {
                grabbed_node.type = NODE_EL_R_HANDLE;
                grabbed_node.node = n;
                break;
            }
            if (CheckCollisionPointCircle(mouse_pos, n->pos, 10)) {
                grabbed_node.type = NODE_EL_NODE;
                grabbed_node.node = n;
                break;
            }
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        grabbed_node.type = NODE_EL_NONE;
    }
}

void track_init(Track *track) {
    track->nodes = ARR(Node, 10);
    track->verts = ARR(V2, 30);
    track->closed = false;
}

void track_destroy(Track *track) {
    arr_free(track->nodes);
    arr_free(track->verts);
}

void track_editor_load(TrackEditorState *state) {
    track_init(&state->track);
}

void track_editor_unload(TrackEditorState *state) {
    track_destroy(&state->track);
}

void track_editor_update(TrackEditorState *state) {
    editor_process_input(state);

    if (grabbed_node.type != NODE_EL_NONE) {
        node_move(grabbed_node.node, grabbed_node.type, mouse_pos);
    }

    track_gen_geo(&state->track);
    track_draw_geo(&state->track);
    track_draw(&state->track);
}

// =======================================================

// static float track_geo_spacing = 20;
// static RenderTexture2D render_tex;
// static bool is_texture_loaded = false;
// static bool first_time = true;
// static NodeElement grabbed_node_el = { .type = NODE_EL_NONE };
// static V2 mouse_pos = {0,0};
// 
// #define TRACK_FOR_EACH_NODE(track_ptr, node_ptr_name) \
//     Node *(node_ptr_name) = &(track_ptr)->nodes[0]; \
//     for (int node_ptr_idx = 0; \
//          node_ptr_idx < ARR_LEN(track_ptr->nodes); \
//          (node_ptr_name) = &(track_ptr)->nodes[++node_ptr_idx])
// 
// 
// void track_gen_geo(Track *track) {
//     if (!is_texture_loaded) {
//         render_tex = LoadRenderTexture(800, 600);
//         is_texture_loaded = true;
//     }
// 
//     ARR_LEN(track->verts) = 0;
//     Node *nodes = track->nodes;
// 
//     for (int i = 0; i < (int)(ARR_LEN(nodes) - 1); ++i) {
//         Node *n  = &nodes[i    ];
//         Node *nn = &nodes[i + 1];
//         V2 p1 = n->pos;
//         V2 c2 = n->ctl_pts[1];
//         V2 c3 = nn->ctl_pts[0];
//         V2 p4 = nn->pos;
//         float t = 0;
//         float delta_t = .1;
// 
//         while (t <= 1) {
//             V2 a = GetSplinePointBezierCubic(p1, c2, c3, p4, t);
//             DrawCircleV(a, 5, MAROON);
//             t += delta_t;
//             V2 b = GetSplinePointBezierCubic(p1, c2, c3, p4, t);
// 
//             float d = v2_dist(a, b);
//             d /= track_geo_spacing;
// 
//             V2 dir = v2_sub(b, a);
//             dir = v2_normalize(dir);
// 
//             static float track_width = 20;
//             V2 transverse_dir_l = v2_mul_scalar(dir, track_width);
//             transverse_dir_l = v2_rot_90_ccw(transverse_dir_l, (V2){0, 0});
//             V2 transverse_dir_r = v2_rot_180(transverse_dir_l, (V2){0, 0});
// 
//             float t2 = 0;
//             while (t2 <= 1) {
//                 V2 p = GetSplinePointLinear(a, b, t2);
//                 V2 transverse_point_l = v2_add(p, transverse_dir_l);
//                 V2 transverse_point_r = v2_add(p, transverse_dir_r);
//                 ARR_APPEND(track->verts, transverse_point_l);
//                 ARR_APPEND(track->verts, transverse_point_r);
// 
//                 DrawCircleV(transverse_point_l, 3, LIME);
//                 DrawCircleV(transverse_point_r, 3, LIME);
//                 DrawCircleV(p, 3, BLUE);
//                 t2 += 1/d;
//             }
//         }
//     }
//     ARR_APPEND(track->verts, track->verts[0]);
//     ARR_APPEND(track->verts, track->verts[1]);
// }
// 
// void track_draw_geo(Track *track) {
//     size_t len = ARR_LEN(track->verts);
//     size_t cap = ARR_CAPACITY(track->verts);
//     ArrHeader *h = ARR_HEADER(track->verts);
//     if (len > 0) {
//         // printf("ARR_LEN(): %zu\n", len);
//         // printf("ARR_CAPACITY(): %zu\n", cap);
//     }
// 
//     BeginTextureMode(render_tex);
//     ClearBackground(RAYWHITE);
// 
//     // DrawTriangleStrip(track->verts, ARR_LEN(track->verts), LIGHTGRAY);
//     for (int i = 0; i < (int)(ARR_LEN(track->verts) - 1); ++i) {
//         V2 va = track->verts[i    ];
//         V2 vb = track->verts[i + 1];
//         DrawLineV(va, vb, BLACK);
//     }
// 
//     EndTextureMode();
// 
//     DrawTextureRec(render_tex.texture,
//                    (Rectangle){
//                         0,
//                         0,
//                         render_tex.texture.width,
//                         -render_tex.texture.height},
//                    (V2){0, 0},
//                    WHITE);
//     Rectangle src_rec = {
//         .x = 0, .y = 0,
//         render_tex.texture.width, -render_tex.texture.height,
//     };
//     Rectangle dst_rec = {
//         .x = 0, .y = 0,
//         100, 100,
//     };
//     DrawTexturePro(render_tex.texture,
//             src_rec,
//             dst_rec,
//                    (V2){0, 0},
//                    0,
//                    RAYWHITE);
// }
// 
// void track_init(Track *track) {
//     track->nodes = ARR(Node, 10);
//     track->verts = ARR(V2, 10);
// 
//     // printf("ARR_LEN(track->nodes): %zu\n", ARR_LEN(track->nodes));
//     // track_add_node_at(track, (V2){0, 0});
//     // printf("ARR_LEN(track->nodes): %zu\n", ARR_LEN(track->nodes));
//     // grabbed_node_el.node = &track->nodes[0];
// }
// 
// void node_destroy(Node *node) {
//     // arr_free(node->verts);
// }
// 
// void track_destroy(Track *track) {
//     printf("track_destroy\n");
//     TRACK_FOR_EACH_NODE(track, n)
//         node_destroy(n);
//     arr_free(track->nodes);
// }
// 
// void set_left_handle_pos(Node *node, V2 pos) {
//     // left handle
//     node->handles[0] = pos;
// 
//     // right handle
//     node->handles[1] = v2_rot_180(pos, node->pos);
// 
//     // left ctl point
//     float d = v2_dist(pos, node->pos);
//     V2 p = v2_sub(pos, node->pos);
//     p = (V2){.x = p.x/d, p.y/d };
//     p = v2_mul_scalar(p, d - 25);
//     p = v2_add(p, node->pos);
//     node->ctl_pts[0] = v2_rot_90_cw(p, node->pos);
// 
//     // right ctl point
//     node->ctl_pts[1] = v2_rot_180(node->ctl_pts[0], node->pos);
// }
// 
// void set_right_handle_pos(Node *node, V2 pos) {
//     V2 left_handle_pos = v2_rot_180(pos, node->pos);
//     set_left_handle_pos(node, left_handle_pos);
// }
// 
// void track_add_node_at(Track *track, V2 pos) {
//     Node node = {
//         .pos = pos,
//         .ctl_pts[0] = pos,
//         .ctl_pts[1] = pos,
//         .handles[0] = pos,
//         .handles[1] = pos,
//     };
//     Node *first_node  = &track->nodes[0];
//     float d = v2_dist(pos, first_node->pos);
//     float snap_threshold_dist = 10;
//     if (ARR_LEN(track->nodes) > 2 && d < snap_threshold_dist) {
//         // snap last node to first
//         ARR_APPEND(track->nodes, *first_node);
//         return;
//     } else
//         ARR_APPEND(track->nodes, node);
// 
//     // If adding the second node, make the first nodes handle
//     // perpendicular to created line
//     if (ARR_LEN(track->nodes) > 1) {
//         Node *second_node = &track->nodes[1];
//         V2 dir = v2_sub(second_node->pos, first_node->pos);
//         dir = v2_normalize(dir);
// 
//         float n = 25;
//         V2 scaled = v2_mul_scalar(dir, n);
//         V2 p = v2_add(first_node->pos, scaled);
//         V2 rotated = v2_rot_90_cw(p, first_node->pos);
// 
//         if (ARR_LEN(track->nodes) == 2) {
//             set_left_handle_pos(first_node, rotated);
//         }
// 
//         // make handles on last node perpendicular
//         Node *last_node = &track->nodes[ARR_LEN(track->nodes) - 1];
//         Node *next_to_last_node = &track->nodes[ARR_LEN(track->nodes) - 2];
//         V2 dir2 = v2_sub(last_node->pos, next_to_last_node->pos);
//         dir2 = v2_normalize(dir2);
//         V2 scaled2 = v2_mul_scalar(dir2, n);
//         V2 p2 = v2_rot_90_cw(scaled2, (V2){0, 0});
//         p2 = v2_add(p2, last_node->pos);
//         set_left_handle_pos(last_node, p2);
//     }
// }
// 
// void track_clear_nodes(Track *track) {
//     arr_clear(track->nodes);
// }
// 
// static void draw_handles(V2 pos_a, V2 pos_b) {
//     DrawCircleV(pos_a, 8, COLOR_HANDLE);
//     DrawCircleV(pos_b, 8, COLOR_HANDLE);
//     DrawLineEx(pos_a, pos_b, 2, BLACK);
// }
// 
// static void draw_node_pt(V2 pos) {
//     DrawCircleV(pos, 10, COLOR_NODE);
// }
// 
// static void draw_ctl_pts(V2 pos_a, V2 pos_b) {
//     DrawCircleV(pos_a, 5, COLOR_CTL_PT);
//     DrawCircleV(pos_b, 5, COLOR_CTL_PT);
// }
// 
// void node_draw(Node *node) {
//     draw_handles(node->handles[0], node->handles[1]);
//     // draw_ctl_pts(node->ctl_pts[0], node->ctl_pts[1]);
//     draw_node_pt(node->pos);
// }
// 
// void track_draw(Track *track) {
//     if (ARR_LEN(track->nodes) > 1) {
//         for (int i = 0; i < (int)(ARR_LEN(track->nodes) - 1); ++i) {
//             V2 p1 = track->nodes[i  ].pos;
//             V2 c2 = track->nodes[i  ].ctl_pts[1];
//             V2 c3 = track->nodes[i+1].ctl_pts[0];
//             V2 p4 = track->nodes[i+1].pos;
//             float thickness = 4;
//             DrawSplineSegmentBezierCubic(p1, c2, c3, p4, thickness, ORANGE);
//         }
//     }
//     track_gen_geo(track);
//     track_draw_geo(track);
// 
//     for (int i = 0; i < ARR_LEN(track->nodes); ++i)
//         node_draw(&track->nodes[i]);
// 
//     // print node positions
//     char buf[99];
//     char buf2[99];
//     for (size_t i = 0; i < ARR_LEN(track->nodes); ++i) {
//         sprintf(buf2, "(%.1f, %.1f)", track->nodes[i].pos.x, track->nodes[i].pos.y);
//         if (i == 0)
//             strcpy(buf, buf2);
//         else {
//             strcat(buf, " -> ");
//             strcat(buf, buf2);
//         }
//     }
//     DrawText(buf, 200, 50, 20, BLACK);
// 
//     // print grabbed_node pos
//     if (grabbed_node_el.type == NODE_EL_NONE)
//         strcpy(buf, "grabbed node el: NONE");
//     else
//         sprintf(buf, "grabbed_node_el: (%.1f, %.1f)", grabbed_node_el.node->pos.x, grabbed_node_el.node->pos.y);
//     DrawText(buf, 200, 100, 20, BLACK);
// }
// 
// void node_move(Node *node, V2 pos) {
//     V2 delta = v2_sub(pos, node->pos);
//     node->pos = pos;
//     node->handles[0] = v2_add(node->handles[0], delta);
//     node->handles[1] = v2_add(node->handles[1], delta);
//     node->ctl_pts[0] = v2_add(node->ctl_pts[0], delta);
//     node->ctl_pts[1] = v2_add(node->ctl_pts[1], delta);
// }
// 
// void start_node_el_drag(NodeElement node_el) {
//     printf("start_node_el_drag\n");
//     grabbed_node_el = node_el;
// }
// 
// void stop_node_el_drag() {
//     printf("stop_node_el_drag\n");
//     grabbed_node_el.type = NODE_EL_NONE;
// }
// 
// NodeElement track_grab_node_el_at(Track *track, V2 pos) {
//     TRACK_FOR_EACH_NODE(track, n) {
//         float check_radius = 20;
//         if (CheckCollisionPointCircle(n->pos, pos, check_radius))
//             return (NodeElement){ NODE_EL_NODE, n };
//         else if (CheckCollisionPointCircle(n->handles[0], pos, check_radius))
//             return (NodeElement){ NODE_EL_L_HANDLE, n };
//         else if (CheckCollisionPointCircle(n->handles[1], pos, check_radius))
//             return (NodeElement){ NODE_EL_R_HANDLE, n };
//     }
// 
//     return (NodeElement){
//         .type = NODE_EL_NONE,
//         .node = NULL,
//     };
// }
// 
// void editor_process_input(TrackEditorState *state) {
//     mouse_pos = GetMousePosition();
// 
//     if (grabbed_node_el.type != NODE_EL_NONE)
//         grabbed_node_el.node->pos = mouse_pos;
// 
//     if (state->editing_mode == MODE_ADDING) {
//         if (IsKeyPressed(KEY_SPACE)) {
//             printf("adding node\n");
//             track_add_node_at(&state->track, mouse_pos);
//         }
//         if (IsKeyPressed(KEY_C))
//             track_clear_nodes(&state->track);
// 
//         // move node
//         if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//             grabbed_node_el = track_grab_node_el_at(&state->track, mouse_pos);
//             const char *grabbed_node_type_str =
//                 grabbed_node_el.type == NODE_EL_NONE
//                     ? "NONE"
//                     : grabbed_node_el.type == NODE_EL_NODE
//                     ? "NODE"
//                     : grabbed_node_el.type == NODE_EL_L_HANDLE
//                     ? "L_HANDLE"
//                     : grabbed_node_el.type == NODE_EL_R_HANDLE
//                     ? "R_HANDLE"
//                     : "???";
//             printf("got: type: %s, pos: (%.1f, %.1f)\n",
//                     grabbed_node_type_str,
//                     grabbed_node_el.type != NODE_EL_NONE ? grabbed_node_el.node->pos.x : -1.0,
//                     grabbed_node_el.type != NODE_EL_NONE ? grabbed_node_el.node->pos.y : -1.0);
//             if (grabbed_node_el.type == NODE_EL_NONE)
//                 start_node_el_drag(grabbed_node_el);
//         }
//         if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) 
//             stop_node_el_drag();
//     }
// 
//     if (IsKeyPressed(KEY_X)) {
//         // toggle_editing_mode();
//     }
// }
// 
// 
// void track_editor_load(TrackEditorState *state) {
//     track_init(&state->track);
// }
// 
// void track_editor_unload(TrackEditorState *state) {
//     track_destroy(&state->track);
// }
// 
// void track_editor_update(TrackEditorState *state) {
//     editor_process_input(state);
// 
//     Track *track = &state->track;
// 
//     Node *first_node = &track->nodes[0];
//     Node *last_node = &track->nodes[ARR_LEN(state->track.nodes) - 1];
//     switch (grabbed_node_el.type) {
//         case NODE_EL_NONE:
//             break;
//         case NODE_EL_NODE:
//             node_move(grabbed_node_el.node, mouse_pos);
//             if (grabbed_node_el.node == last_node)
//                 node_move(first_node, mouse_pos);
//             else if (grabbed_node_el.node == first_node)
//                 node_move(last_node, mouse_pos);
//             break;
//         case NODE_EL_L_HANDLE:
//             set_left_handle_pos(grabbed_node_el.node, mouse_pos);
//             if (grabbed_node_el.node == last_node)
//                 set_left_handle_pos(first_node, mouse_pos);
//             else if (grabbed_node_el.node == first_node)
//                 set_left_handle_pos(last_node, mouse_pos);
//             break;
//         case NODE_EL_R_HANDLE:
//             set_right_handle_pos(grabbed_node_el.node, mouse_pos);
//             if (grabbed_node_el.node == last_node)
//                 set_right_handle_pos(first_node, mouse_pos);
//             else if (grabbed_node_el.node == first_node)
//                 set_right_handle_pos(last_node, mouse_pos);
//             break;
//     }
// 
//     track_draw(track);
// }
