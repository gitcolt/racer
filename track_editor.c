#include "track_editor.h"
#include "colors.h"
#define DYN_ARR_IMPL
#include "shared/dyn_arr.h"
#include "shared/mymath.h"

#include "rlgl.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>

static float track_geo_spacing = 20;
static RenderTexture2D render_tex;
static bool is_texture_loaded = false;
static bool first_time = true;
static NodeElement grabbed_node_el = { .type = NODE_EL_NODE };

#define TRACK_FOR_EACH_NODE(track_ptr, node_ptr_name) \
    Node *(node_ptr_name) = &(track_ptr)->nodes[0]; \
    for (int node_ptr_idx = 0; \
         node_ptr_idx < ARR_LEN(track_ptr->nodes); \
         (node_ptr_name) = &(track_ptr)->nodes[++node_ptr_idx])

// TODO: Make track a static

void editor_process_input(Track *track) {
}

void track_gen_geo(Track *track) {
    if (!is_texture_loaded) {
        render_tex = LoadRenderTexture(800, 600);
        is_texture_loaded = true;
    }

    ARR_LEN(track->verts) = 0;
    Node *nodes = track->nodes;

    for (int i = 0; i < (int)(ARR_LEN(nodes) - 1); ++i) {
        Node *n  = &nodes[i    ];
        Node *nn = &nodes[i + 1];
        V2 p1 = n->pos;
        V2 c2 = n->ctl_pts[1];
        V2 c3 = nn->ctl_pts[0];
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
            while (t2 <= 1) {
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
    ARR_APPEND(track->verts, track->verts[0]);
    ARR_APPEND(track->verts, track->verts[1]);
}

void track_draw_geo(Track *track) {
    size_t len = ARR_LEN(track->verts);
    size_t cap = ARR_CAPACITY(track->verts);
    ArrHeader *h = ARR_HEADER(track->verts);
    if (len > 0) {
        printf("ARR_LEN(): %zu\n", len);
        printf("ARR_CAPACITY(): %zu\n", cap);
    }

    BeginTextureMode(render_tex);
    ClearBackground(RAYWHITE);

    DrawTriangleStrip(track->verts, ARR_LEN(track->verts), LIGHTGRAY);
    // for (int i = 0; i < (int)(ARR_LEN(track->verts) - 1); ++i) {
    //     V2 va = track->verts[i    ];
    //     V2 vb = track->verts[i + 1];
    //     DrawLineV(va, vb, BLACK);
    // }

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

void track_init(Track *track) {
    track->nodes = ARR(Node, 10);
    track->verts = ARR(V2, 10);
}

void node_destroy(Node *node) {
    // arr_free(node->verts);
}

void track_destroy(Track *track) {
    printf("track_destroy\n");
    TRACK_FOR_EACH_NODE(track, n)
        node_destroy(n);
    arr_free(track->nodes);
}

void set_left_handle_pos(Node *node, V2 pos) {
    // left handle
    node->handles[0] = pos;

    // right handle
    node->handles[1] = v2_rot_180(pos, node->pos);

    // left ctl point
    float d = v2_dist(pos, node->pos);
    V2 p = v2_sub(pos, node->pos);
    p = (V2){.x = p.x/d, p.y/d };
    p = v2_mul_scalar(p, d - 25);
    p = v2_add(p, node->pos);
    node->ctl_pts[0] = v2_rot_90_cw(p, node->pos);

    // right ctl point
    node->ctl_pts[1] = v2_rot_180(node->ctl_pts[0], node->pos);
}

void set_right_handle_pos(Node *node, V2 pos) {
    V2 left_handle_pos = v2_rot_180(pos, node->pos);
    set_left_handle_pos(node, left_handle_pos);
}

void track_add_node_at(Track *track, V2 pos) {
    Node node = {
        .pos = pos,
        .ctl_pts[0] = pos,
        .ctl_pts[1] = pos,
        .handles[0] = pos,
        .handles[1] = pos,
    };
    Node *first_node  = &track->nodes[0];
    float d = v2_dist(pos, first_node->pos);
    float snap_threshold_dist = 10;
    if (ARR_LEN(track->nodes) > 2 && d < snap_threshold_dist) {
        // snap last node to first
        ARR_APPEND(track->nodes, *first_node);
        return;
    } else
        ARR_APPEND(track->nodes, node);

    // If adding the second node, make the first nodes handle
    // perpendicular to created line
    if (ARR_LEN(track->nodes) > 1) {
        Node *second_node = &track->nodes[1];
        V2 dir = v2_sub(second_node->pos, first_node->pos);
        dir = v2_normalize(dir);

        float n = 25;
        V2 scaled = v2_mul_scalar(dir, n);
        V2 p = v2_add(first_node->pos, scaled);
        V2 rotated = v2_rot_90_cw(p, first_node->pos);

        if (ARR_LEN(track->nodes) == 2) {
            set_left_handle_pos(first_node, rotated);
        }

        // make handles on last node perpendicular
        Node *last_node = &track->nodes[ARR_LEN(track->nodes) - 1];
        Node *next_to_last_node = &track->nodes[ARR_LEN(track->nodes) - 2];
        V2 dir2 = v2_sub(last_node->pos, next_to_last_node->pos);
        dir2 = v2_normalize(dir2);
        V2 scaled2 = v2_mul_scalar(dir2, n);
        V2 p2 = v2_rot_90_cw(scaled2, (V2){0, 0});
        p2 = v2_add(p2, last_node->pos);
        set_left_handle_pos(last_node, p2);
    }
}

void track_clear_nodes(Track *track) {
    arr_clear(track->nodes);
}

static void draw_handles(V2 pos_a, V2 pos_b) {
    DrawCircleV(pos_a, 8, COLOR_HANDLE);
    DrawCircleV(pos_b, 8, COLOR_HANDLE);
    DrawLineEx(pos_a, pos_b, 2, BLACK);
}

static void draw_node_pt(V2 pos) {
    DrawCircleV(pos, 10, COLOR_NODE);
}

static void draw_ctl_pts(V2 pos_a, V2 pos_b) {
    DrawCircleV(pos_a, 5, COLOR_CTL_PT);
    DrawCircleV(pos_b, 5, COLOR_CTL_PT);
}

void node_draw(Node *node) {
    draw_handles(node->handles[0], node->handles[1]);
    // draw_ctl_pts(node->ctl_pts[0], node->ctl_pts[1]);
    draw_node_pt(node->pos);
}

void track_draw(Track *track) {
    if (ARR_LEN(track->nodes) > 1) {
        for (int i = 0; i < (int)(ARR_LEN(track->nodes) - 1); ++i) {
            V2 p1 = track->nodes[i  ].pos;
            V2 c2 = track->nodes[i  ].ctl_pts[1];
            V2 c3 = track->nodes[i+1].ctl_pts[0];
            V2 p4 = track->nodes[i+1].pos;
            float thickness = 4;
            DrawSplineSegmentBezierCubic(p1, c2, c3, p4, thickness, ORANGE);
        }
    }
    track_gen_geo(track);
    track_draw_geo(track);

    for (int i = 0; i < ARR_LEN(track->nodes); ++i)
        node_draw(&track->nodes[i]);
}

void node_move(Node *node, V2 pos) {
    V2 delta = v2_sub(pos, node->pos);
    node->pos = pos;
    node->handles[0] = v2_add(node->handles[0], delta);
    node->handles[1] = v2_add(node->handles[1], delta);
    node->ctl_pts[0] = v2_add(node->ctl_pts[0], delta);
    node->ctl_pts[1] = v2_add(node->ctl_pts[1], delta);
}

void start_node_el_drag(NodeElement node_el) {
    grabbed_node_el = node_el;
}

void stop_node_el_drag() {
    grabbed_node_el.type = NODE_EL_NONE;
}

NodeElement track_grab_node_el_at(Track *track, V2 pos) {
    TRACK_FOR_EACH_NODE(track, n) {
        float check_radius = 20;
        if (CheckCollisionPointCircle(n->pos, pos, check_radius))
            return (NodeElement){ NODE_EL_NODE, n };
        else if (CheckCollisionPointCircle(n->handles[0], pos, check_radius))
            return (NodeElement){ NODE_EL_L_HANDLE, n };
        else if (CheckCollisionPointCircle(n->handles[1], pos, check_radius))
            return (NodeElement){ NODE_EL_R_HANDLE, n };
    }

    return (NodeElement){
        .type = NODE_EL_NONE,
        .node = NULL,
    };
}
