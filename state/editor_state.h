#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "../shared.h"
#include "../track_editor.h"

typedef struct {
    Track track;
    enum TrackEditorMode editing_mode;
} EditorState;

#endif
