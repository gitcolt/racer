#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "../track_editor.h"

typedef struct {
    Track track;
    enum TrackEditorMode editing_mode;
} EditorState;

#endif
