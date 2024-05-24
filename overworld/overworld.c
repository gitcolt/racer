#include "overworld.h"
#include "layer_names.h"
#include "../tiles.h"
#include "../dialog/dialog.h"
#include "../dialog/dialog_parser.h"
#include "../collision.h"
#include "../shared/dyn_arr.h"
#include "../shared/q.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static V2 map_offset = { 0, 0 };
static bool show_dialog = false;
static bool hit = true;
static Conversation *curr_convo;
static Q npc_collision_q;

typedef struct {
    V2 s;
    V2 e;
} LineSeg;

void player_collide(OverworldPlayer *player, BoxCollider collider) {
    V2 pos = player->collider.center;
    Rectangle rec = collider.rec;
    LineSeg top = {
        { rec.x,             rec.y },
        { rec.x + rec.width, rec.y },
    };
    LineSeg bottom = {
        { rec.x,             rec.y + rec.height },
        { rec.x + rec.width, rec.y + rec.height },
    };
    LineSeg left = {
        { rec.x, rec.y              },
        { rec.x, rec.y + rec.height },
    };
    LineSeg right = {
        { rec.x + rec.width, rec.y              },
        { rec.x + rec.width, rec.y + rec.height },
    };

    if (top.s.y > pos.y && top.s.y < (pos.y + PLAYER_COLLISION_RADIUS)) {
        player->pos.y = top.s.y - PLAYER_COLLISION_RADIUS;
    } else if (bottom.s.y > (pos.y - PLAYER_COLLISION_RADIUS) && bottom.s.y < pos.y) {
        player->pos.y = bottom.s.y + PLAYER_COLLISION_RADIUS;
    }
    if (left.s.x > pos.x && left.s.x < (pos.x + PLAYER_COLLISION_RADIUS)) {
        player->pos.x = left.s.x - PLAYER_COLLISION_RADIUS;
    } else if (right.s.x > (pos.x - PLAYER_COLLISION_RADIUS) && right.s.x < pos.x) {
        player->pos.x = right.s.x + PLAYER_COLLISION_RADIUS;
    }
}

void overworld_process_tile_collisions(BoxCollider *colliders, size_t colliders_size, OverworldPlayer *player) {
    for (size_t i = 0; i < colliders_size; ++i) {
        BoxCollider bc = colliders[i];
        hit = CheckCollisionCircleRec(player->pos, PLAYER_COLLISION_RADIUS, bc.rec);
        if (hit) {
            player_collide(player, bc);
            // break;
        }
    }
}

void draw_colliders(BoxCollider *colliders, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        Rectangle rec = {
            colliders[i].rec.x + map_offset.x,
            colliders[i].rec.y + map_offset.y,
            colliders[i].rec.width,
            colliders[i].rec.height,
        };
        DrawRectangleRec(rec, RED);
    }
}

void overworld_npcs_draw(NPC *npcs, size_t num_npcs) {
    for (size_t i = 0; i < num_npcs; ++i) {
        npc_draw(&npcs[i], map_offset);
    }
}

void overworld_draw(OverworldState *state) {
    draw_tiles(&state->bg_tile_data, map_offset);
    draw_tiles(&state->mg_tile_data, map_offset);
    overworld_player_draw(&state->player, map_offset);
    overworld_npcs_draw(state->npcs, state->num_npcs);
    draw_tiles(&state->fg_tile_data, map_offset);
    // draw_colliders(state->colliders, ARR_LEN(state->colliders));
    draw_grid(map_offset);

    if (hit) {
        const char *text = "HIT";
        int font_size = 30;
        DrawText(text, SCREEN_WIDTH - MeasureText(text, font_size) - 20, 100, font_size, DARKPURPLE);
    }
}

void overworld_process_input(OverworldState *state) {
    curr_convo = &state->convos.convos[0];
    state->mouse_pos = GetMousePosition();

    if (show_dialog) {
        // control dialog
        if (IsKeyPressed(KEY_UP))
            conversation_prev_choice(curr_convo);
        else if (IsKeyPressed(KEY_DOWN))
            conversation_next_choice(curr_convo);
        else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            show_dialog =
                conversation_continue(curr_convo);
        }

    } else {
        // control player
        bool key_up = IsKeyDown(KEY_UP);
        bool key_down = IsKeyDown(KEY_DOWN);
        bool key_left = IsKeyDown(KEY_LEFT);
        bool key_right = IsKeyDown(KEY_RIGHT);
        const float pan_speed = state->player.walk_speed;
        if (key_up) {
            overworld_player_set_dir(&state->player, DIR_UP);
        }
        if (key_down) {
            overworld_player_set_dir(&state->player, DIR_DOWN);
        }
        if (key_left) {
            overworld_player_set_dir(&state->player, DIR_LEFT);
        }
        if (key_right) {
            overworld_player_set_dir(&state->player, DIR_RIGHT);
        }

        if ( !key_up && !key_down && !key_left && !key_right )
            overworld_player_stop_moving(&state->player);

        overworld_player_move(&state->player);

        if (IsKeyPressed(KEY_D))
            show_dialog = !show_dialog;

        map_offset.x = 300 - state->player.pos.x;
        map_offset.y = 300 - state->player.pos.y;
    }
}

void overworld_npcs_update(NPC *npcs, size_t num_npcs, Tick tick) {
    for (size_t i = 0; i < num_npcs; ++i)
        npc_update(&npcs[i], tick);
}

void overworld_process_entity_collisions(OverworldState *state) {
    for (size_t i = 0; i < state->num_npcs; ++i) {
        NPC *npc = &state->npcs[i];
        if (CheckCollisionCircles(npc->collider.center,
                                  npc->collider.radius,
                                  state->player.collider.center,
                                  state->player.collider.radius))
            printf("Collision!!!!!!!!\n");
    }
}

void overworld_update(OverworldState *state) {
    overworld_process_input(state);
    overworld_player_update(&state->player, state->tick);
    overworld_npcs_update(state->npcs, state->num_npcs, state->tick);
    overworld_process_tile_collisions(state->colliders,
                                      ARR_LEN(state->colliders),
                                      &state->player);
    overworld_process_entity_collisions(state);
    overworld_draw(state);

    if (show_dialog) {
        Rectangle dialog_rec = {
            .x = 100,
            .y = 100,
            .width = 300,
            .height = 300,
        };
        conversation_show(&state->convos.convos[0], dialog_rec);
    }
}

void overworld_load(OverworldState *state) {
    // load map data
    const char *map_data_filename = "LDtk_files/overworld.ldtk";
    loadJSONFile("{\"jsonVersion\":\"\"}", (char *)map_data_filename);
    importMapData();

    struct levels *lvl_one = getLevel("Level_0");
    state->lvl_one = lvl_one;

    Texture2D tilemap_tex = LoadTexture("assets/overworld_tilemap.png");

    struct layerInstances *bg_layer = getLayer(LAYER_NAME_BG, lvl_one->uid);
    state->bg_tile_data.tiles = bg_layer->gridTiles_data_ptr;
    state->bg_tile_data.tilemap_tex = tilemap_tex;
    state->bg_tile_data.src_tile_sz = bg_layer->gridSize;

    struct layerInstances *mg_layer = getLayer(LAYER_NAME_MG, lvl_one->uid);
    state->mg_tile_data.tiles = mg_layer->gridTiles_data_ptr;
    state->mg_tile_data.tilemap_tex = tilemap_tex;
    state->mg_tile_data.src_tile_sz = mg_layer->gridSize;

    struct layerInstances *fg_layer = getLayer(LAYER_NAME_FG, lvl_one->uid);
    state->fg_tile_data.tiles = fg_layer->gridTiles_data_ptr;
    state->fg_tile_data.tilemap_tex = tilemap_tex;
    state->fg_tile_data.src_tile_sz = fg_layer->gridSize;

    struct layerInstances *collisions_layer = getLayer(LAYER_NAME_COLLISIONS,
                                                       lvl_one->uid);
    int *collisions = collisions_layer->intGrid;
    state->colliders = ARR(BoxCollider, 10);
    bool x = false;
    for (int row = 0; row < 17; ++row) {
        for (int col = 0; col < 17; ++col) {
            if (!collisions[17 * row + col])
                continue;
            BoxCollider bc = {
                .rec = {
                    .x = col * DST_TILE_SIZE,
                    .y = row * DST_TILE_SIZE,
                    .width = DST_TILE_SIZE,
                    .height = DST_TILE_SIZE,
                },
            };
            ARR_APPEND(state->colliders, bc);
        }
    }

    struct layerInstances *entities_layer = getLayer(LAYER_NAME_ENTITIES, lvl_one->uid);
    struct entityInstances *entities = entities_layer->entityInstances_data_ptr;
    int num_entities = entities_layer->numEntityInstancesDataPtr;
    state->npcs = ARR(NPC, num_entities);
    state->num_npcs = num_entities;
    for (int i = 0; i < num_entities; ++i) {
        NPC npc = {};
        Conversation *convo = NULL;
        struct entityInstances *ent = &entities[i];
        for (int j = 0; j < ent->fieldInstanceCount; ++j) {
            struct fieldInstances *fi = &ent->fieldInstances_data_ptr[j];
            if (!strcmp(fi->type, "String") &&
                !strcmp(fi->identifier, "conversation_id")) {
                struct fieldInstances_strings *str_fields =
                    fi->fieldInstances_strings_ptr;
                for (int k = 0; k < str_fields->size; ++k) {
                    const char *convo_id = str_fields[k].strValue;
                    printf("Entity '%s's convo_id is '%s'\n", ent->identifier, convo_id);
                    convo = get_convo_by_id(&state->convos, convo_id);
                }
            } else {
                fprintf(stderr, "Invalid entity field type or identifier\n");
                exit(EXIT_FAILURE);
            }
        }
        npc_load(&npc,
                 (V2){ ent->cx, ent->cy },
                 ent->identifier,
                 convo);
        ARR_APPEND(state->npcs, npc);
    }

    overworld_player_load(&state->player);

    load_conversations_from_xml(&state->convos, "dialog/test_dialog.xml");
    // conversations_print(&state->convos);
}

void overworld_npcs_unload(NPC *npcs, size_t num_npcs) {
    for (size_t i = 0; i < num_npcs; ++i)
        npc_unload(&npcs[i]);
}

void overworld_unload(OverworldState *state) {
    printf("overworld_unload()\n");
    arr_free(state->colliders);
    arr_free(state->npcs);
    UnloadTexture(state->bg_tile_data.tilemap_tex);
    UnloadTexture(state->mg_tile_data.tilemap_tex);
    UnloadTexture(state->fg_tile_data.tilemap_tex);
    overworld_player_unload(&state->player);
    overworld_npcs_unload(state->npcs, state->num_npcs);
    unload_conversations(&state->convos);

    freeMapData();
    json_value_free(schema);
    json_value_free(user_data);
}
