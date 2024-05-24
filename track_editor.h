#ifndef TRACK_EDITOR_H
#define TRACK_EDITOR_H

#include "shared/shared.h"

#include <stdlib.h>

enum TrackEditorMode {
    MODE_ADDING,
    MODE_MOVING,
};

typedef struct {
    V2 ctl_pts[2];
    V2 handles[2];
    V2 pos;
    int nverts;
} Node;

typedef struct {
    Node *nodes;
    V2 *verts;
} Track;

enum NodeElementType {
    NODE_EL_NONE,
    NODE_EL_NODE,
    NODE_EL_L_HANDLE,
    NODE_EL_R_HANDLE,
};

typedef struct {
    enum NodeElementType type;
    Node *node;
} NodeElement;

void track_init(Track *track);
void track_destroy(Track *track);
void track_add_node_at(Track *track, V2 pos);
void track_clear_nodes(Track *track);
void track_draw(Track *track);
void set_left_handle_pos(Node *node, V2 pos);
void set_right_handle_pos(Node *node, V2 pos);
void node_move(Node *node, V2 pos);
NodeElement track_grab_node_el_at(Track *track, V2 pos);

#endif
