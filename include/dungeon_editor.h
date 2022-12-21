#ifndef __DUNGEON_EDITOR_H__
#define __DUNGEON_EDITOR_H__
#include "world.h"
#include "enemy.h"
#include "gfc_primitives.h"
#include <simple_json.h>
#include <simple_logger.h>

typedef enum {
    EM_COLLISION,
    EM_HAZARD,
    EM_ENEMY,
    EM_GEM,
    EM_EQUIP,
    EM_PLAYER_SPAWN,
    EDIT_MODE_MAX
} EditorMode;

typedef struct {
    EnemyType type;
    Vector3D position;
} EnemySpawn;

typedef struct {
    int type;
    Vector3D position;
} HazardSpawn;

typedef struct {
    int current_col_box;

    int enemy_count;
    EnemySpawn* enemies;

    int gem_count;
    Vector3D* gem_spawns;

    int equip_count;
    Vector3D* equip_spawns;

    int hazard_count;
    HazardSpawn* hazards;

    SJson* dungeon_config;

    Model* box_model;
    
    Vector3D camera_lookat_pos;
    Vector3D player_spawn;

    TextLine save_path;
    TextLine model_path;
    TextLine texture_path;
} DungeonEditor;

void dungeon_editor_init(char* config_path);
void dungeon_editor_update();
void dungeon_editor_draw();

#endif