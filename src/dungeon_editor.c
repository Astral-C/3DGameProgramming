#include "dungeon_editor.h"
#include "gfc_input.h"
#include "menu.h"
#include "equipment.h"

static DungeonEditor editor = {0};
static EditorMode mode = EM_COLLISION;
static EnemyType add_enemy_type = Rockman;
static int add_hazard_type = 0;

// oh my god NO

Model* rockman_model = NULL;
Model* zombie_model = NULL;
Model* skeleton_model = NULL;
Model* slime_model = NULL;
Model* goblin_model = NULL;

Model* gem_model = NULL;

Model* spike_hazard_model = NULL;
Model* puddle_hazard_model = NULL;
Model* fire_hazard_model = NULL;

void click_add(){
    switch (mode)
    {
    case EM_COLLISION:
        dungeon.walkable_count++;
        dungeon.walkable = realloc(dungeon.walkable, sizeof(Box) * dungeon.walkable_count);

        dungeon.walkable[dungeon.walkable_count-1].x = 0;
        dungeon.walkable[dungeon.walkable_count-1].y = 0;
        dungeon.walkable[dungeon.walkable_count-1].z = 0;

        dungeon.walkable[dungeon.walkable_count-1].w = 0;
        dungeon.walkable[dungeon.walkable_count-1].h = 0;
        dungeon.walkable[dungeon.walkable_count-1].d = 0;

        slog("dungeon has %d boxes", dungeon.walkable_count);
        break;

    case EM_HAZARD:
        editor.hazard_count++;
        editor.hazards = realloc(editor.hazards, sizeof(HazardSpawn) * editor.hazard_count);
        editor.hazards[editor.hazard_count-1].type = add_hazard_type;

        editor.hazards[editor.hazard_count-1].position.x = 0;
        editor.hazards[editor.hazard_count-1].position.y = 0;
        editor.hazards[editor.hazard_count-1].position.z = 0;

        break;

    case EM_ENEMY:
        editor.enemy_count++;
        editor.enemies = realloc(editor.enemies, sizeof(EnemySpawn) * editor.enemy_count);
        editor.enemies[editor.enemy_count-1].type = add_enemy_type;

        editor.enemies[editor.enemy_count-1].position.x = 0;
        editor.enemies[editor.enemy_count-1].position.y = 0;
        editor.enemies[editor.enemy_count-1].position.z = 0;

        break;

    case EM_GEM:
        editor.gem_count++;
        editor.gem_spawns = realloc(editor.gem_spawns, sizeof(Vector3D) * editor.gem_count);
        editor.gem_spawns[editor.gem_count-1].x = 0;
        editor.gem_spawns[editor.gem_count-1].y = 0;
        editor.gem_spawns[editor.gem_count-1].z = 0;
        break;

    case EM_EQUIP:
        editor.equip_count++;
        editor.equip_spawns = realloc(editor.equip_spawns, sizeof(Vector3D) * editor.equip_count);
        editor.equip_spawns[editor.equip_count-1].x = 0;
        editor.equip_spawns[editor.equip_count-1].y = 0;
        editor.equip_spawns[editor.equip_count-1].z = 0;
        break;

    default:
        break;
    }
}

void click_remove(){

    switch (mode)
    {
    case EM_COLLISION:
        if(dungeon.walkable_count == 0) return;
        memmove(&dungeon.walkable[editor.current_col_box], &dungeon.walkable[editor.current_col_box+1], sizeof(Box));
        dungeon.walkable--;
        break;

    case EM_HAZARD:
        if(editor.hazard_count == 0) return;
        memmove(&editor.hazards[editor.current_col_box], &editor.hazards[editor.current_col_box+1], sizeof(HazardSpawn));
        editor.hazard_count--;
        break;

    case EM_ENEMY:
        if(editor.enemy_count == 0) return;
        memmove(&editor.enemies[editor.current_col_box], &editor.enemies[editor.current_col_box+1], sizeof(EnemySpawn));
        editor.enemy_count--;
        break;

    case EM_GEM:
        if(editor.gem_count == 0) return;
        memmove(&editor.gem_spawns[editor.current_col_box], &editor.gem_spawns[editor.current_col_box+1], sizeof(Vector3D));
        editor.gem_count--;
        break;

    case EM_EQUIP:
        if(editor.gem_count == 0) return;
        memmove(&editor.gem_spawns[editor.current_col_box], &editor.gem_spawns[editor.current_col_box+1], sizeof(Vector3D));
        editor.gem_count--;
        break;

    default:
        break;
    }
}

void click_dungeon_save(){
    
}

void change_editor_mode(){
    mode = (mode + 1) % EDIT_MODE_MAX;

    switch (mode)
    {
    case EM_COLLISION:
        if(editor.current_col_box >= dungeon.walkable_count) editor.current_col_box = 0;
        break;
    case EM_ENEMY:
        if(editor.current_col_box >= editor.enemy_count) editor.current_col_box = 0;
        break;
    case EM_GEM:
        if(editor.current_col_box >= editor.gem_count) editor.current_col_box = 0;
        break;
    case EM_HAZARD:
        if(editor.current_col_box >= editor.hazard_count) editor.current_col_box = 0;
        break; 
    case EM_EQUIP:
        if(editor.current_col_box >= editor.equip_count) editor.current_col_box = 0;
        break;  
    default:
        break;
    }
}

void change_enemy_type(){
    if(mode == EM_ENEMY){
        editor.enemies[editor.current_col_box].type = (editor.enemies[editor.current_col_box].type + 1) % ENEMY_TYPE_MAX;
    }
}

void change_hazard_type(){
    if(mode == EM_HAZARD){
        editor.hazards[editor.current_col_box].type = (editor.hazards[editor.current_col_box].type + 1) % 3;
    }
}

void dungeon_editor_cleanup(){
    if(dungeon.walkable != NULL) free(dungeon.walkable);
    if(editor.enemies != NULL) free(editor.enemies);
    if(editor.gem_spawns != NULL) free(editor.gem_spawns);
    if(editor.hazards != NULL) free(editor.hazards);
    if(editor.equip_spawns != NULL) free(editor.equip_spawns);
}

void dungeon_editor_init(char* config_path){
    slog(config_path);
    editor.dungeon_config = sj_load(config_path);

    char* model_path = sj_get_string_value(sj_object_get_value(editor.dungeon_config, "model"));
    char* texture_path = sj_get_string_value(sj_object_get_value(editor.dungeon_config, "texture"));

    if(model_path != NULL && texture_path != NULL) dungeon.floor = gf3d_model_load_full(model_path, texture_path);
    editor.box_model = gf3d_model_load_full("models/point.obj", "images/default.png");
    gfc_matrix_identity(dungeon.mat);

    SJson* boxes = sj_object_get_value(editor.dungeon_config, "collision");
    SJson* enemy_spawns = sj_object_get_value(editor.dungeon_config, "enemy_spawns");
    SJson* hazard_spawns = sj_object_get_value(editor.dungeon_config, "hazard_spawns");
    SJson* gem_spawns = sj_object_get_value(editor.dungeon_config, "gem_spawns");
    SJson* equip_spawns = sj_object_get_value(editor.dungeon_config, "equip_spawns");

    if(boxes){
        dungeon.walkable_count = sj_array_get_count(boxes);
        dungeon.walkable = malloc(sizeof(Box)*dungeon.walkable_count);
        for (size_t i = 0; i < dungeon.walkable_count; i++){
            SJson* current_box = sj_array_get_nth(boxes, i);
            float x, y, z, w, h, d;
            sj_get_float_value(sj_array_get_nth(current_box, 0), &x);
            sj_get_float_value(sj_array_get_nth(current_box, 1), &y);
            sj_get_float_value(sj_array_get_nth(current_box, 2), &z);
            sj_get_float_value(sj_array_get_nth(current_box, 3), &w);
            sj_get_float_value(sj_array_get_nth(current_box, 4), &h);
            sj_get_float_value(sj_array_get_nth(current_box, 5), &d);

            dungeon.walkable[i] = gfc_box(x,y,z,w,h,d);
        }
    }

    if(enemy_spawns){
        editor.enemy_count = sj_array_get_count(enemy_spawns);
        editor.enemies = malloc(sizeof(EnemySpawn)*editor.enemy_count);
        for (size_t i = 0; i < editor.enemy_count; i++){
            SJson* current = sj_array_get_nth(enemy_spawns, i);
            EnemyType type;
            Vector3D pos;
            sj_get_integer_value(sj_object_get_value(current, "type"), &type);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 0), &pos.x);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 1), &pos.y);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 2), &pos.z);

            editor.enemies[i] = (EnemySpawn){type, pos};
        }
    }
    
    if(hazard_spawns){
        editor.hazard_count = sj_array_get_count(hazard_spawns);
        editor.hazards = malloc(sizeof(HazardSpawn)*editor.hazard_count);
        for (size_t i = 0; i < editor.hazard_count; i++){
            SJson* current = sj_array_get_nth(hazard_spawns, i);
            int type;
            Vector3D pos;
            sj_get_integer_value(sj_object_get_value(current, "type"), &type);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 0), &pos.x);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 1), &pos.y);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 2), &pos.z);

            editor.hazards[i] = (HazardSpawn){type % 3, pos};
        }
    }

    if(gem_spawns){
        editor.gem_count = sj_array_get_count(gem_spawns);
        editor.gem_spawns = malloc(sizeof(Vector3D)*editor.gem_count);
        for (size_t i = 0; i < editor.gem_count; i++){
            SJson* current = sj_array_get_nth(gem_spawns, i);

            sj_get_float_value(sj_array_get_nth(current, 0), &editor.gem_spawns[i].x);
            sj_get_float_value(sj_array_get_nth(current, 1), &editor.gem_spawns[i].y);
            sj_get_float_value(sj_array_get_nth(current, 2), &editor.gem_spawns[i].z);
        }
    }


    rockman_model = gf3d_model_load_full("models/cube.obj", "images/rockman.png");
    zombie_model = gf3d_model_load_full("models/cube.obj", "images/zombie.png");
    skeleton_model = gf3d_model_load_full("models/cube.obj", "images/skeleton.png");
    slime_model = gf3d_model_load_full("models/cube.obj", "images/slime.png");
    goblin_model = gf3d_model_load_full("models/cube.obj", "images/goblin.png");

    gem_model = gf3d_model_load_full("models/gem.obj", "images/gem.png");

    spike_hazard_model = gf3d_model_load_full("models/water_hazard_surface.obj", "images/water_overlay.png");
    puddle_hazard_model = gf3d_model_load_full("models/water_hazard_rim.obj", "images/dirt.png");
    fire_hazard_model = gf3d_model_load_full("models/cube.obj", "images/fireball.png");

    menu_init(10, NULL, NULL);
    menu_add_button("Add", gfc_rect(0,0,0,0), gfc_color(1,1,1,1), click_add);
    menu_add_button("Remove", gfc_rect(0,100,0,0), gfc_color(1,1,1,1), click_remove);
    menu_add_button("Change Mode", gfc_rect(0,200,0,0), gfc_color(1,1,1,1), change_editor_mode);
    menu_add_button("Change Enemy Type", gfc_rect(0,300,0,0), gfc_color(1,1,1,1), change_enemy_type);
    menu_add_button("Change Hazard Type", gfc_rect(0,400,0,0), gfc_color(1,1,1,1), change_enemy_type);
    menu_add_button("Save", gfc_rect(0,600,0,0), gfc_color(1,1,1,1), click_dungeon_save);

}

void dungeon_editor_update(){
    menu_update();

    if(gfc_input_key_pressed("TAB")){
        editor.current_col_box += 1;

        switch (mode)
        {
        case EM_COLLISION:
            if(editor.current_col_box >= dungeon.walkable_count) editor.current_col_box = 0;
            break;
        case EM_ENEMY:
            if(editor.current_col_box >= editor.enemy_count) editor.current_col_box = 0;
            break;
        case EM_GEM:
            if(editor.current_col_box >= editor.gem_count) editor.current_col_box = 0;
            break;
        case EM_HAZARD:
            if(editor.current_col_box >= editor.hazard_count) editor.current_col_box = 0;
            break;   
        default:
            break;
        }
    }

    if(!gfc_input_key_down("LSHIFT")){
        if(gfc_input_key_down("w")){
            editor.camera_lookat_pos.y += 0.5f;
        }

        if(gfc_input_key_down("s")){
            editor.camera_lookat_pos.y -= 0.5f;
        }

        if(gfc_input_key_down("a")){
            editor.camera_lookat_pos.x += 0.5f;
        }

        if(gfc_input_key_down("d")){
            editor.camera_lookat_pos.x -= 0.5f;
        }
    } else {
        switch (mode){
            case EM_ENEMY:
                if(gfc_input_key_down("w")){
                    editor.enemies[editor.current_col_box].position.y += 0.1f;
                }

                if(gfc_input_key_down("s")){
                     editor.enemies[editor.current_col_box].position.y -= 0.1f;
                }

                if(gfc_input_key_down("a")){
                    editor.enemies[editor.current_col_box].position.x += 0.1f;
                }

                if(gfc_input_key_down("d")){
                    editor.enemies[editor.current_col_box].position.x -= 0.1f;
                }
                break;

            case EM_HAZARD:
                if(gfc_input_key_down("w")){
                    editor.hazards[editor.current_col_box].position.y += 0.1f;
                }

                if(gfc_input_key_down("s")){
                     editor.hazards[editor.current_col_box].position.y -= 0.1f;
                }

                if(gfc_input_key_down("a")){
                    editor.hazards[editor.current_col_box].position.x += 0.1f;
                }

                if(gfc_input_key_down("d")){
                    editor.hazards[editor.current_col_box].position.x -= 0.1f;
                }
                break;

            case EM_GEM:
                if(gfc_input_key_down("w")){
                    editor.gem_spawns[editor.current_col_box].y += 0.1f;
                }

                if(gfc_input_key_down("s")){
                     editor.gem_spawns[editor.current_col_box].y -= 0.1f;
                }

                if(gfc_input_key_down("a")){
                    editor.gem_spawns[editor.current_col_box].x += 0.1f;
                }

                if(gfc_input_key_down("d")){
                    editor.gem_spawns[editor.current_col_box].x -= 0.1f;
                }
                break;

            case EM_EQUIP:
                if(gfc_input_key_down("w")){
                    editor.equip_spawns[editor.current_col_box].y += 0.1f;
                }

                if(gfc_input_key_down("s")){
                     editor.equip_spawns[editor.current_col_box].y -= 0.1f;
                }

                if(gfc_input_key_down("a")){
                    editor.equip_spawns[editor.current_col_box].x += 0.1f;
                }

                if(gfc_input_key_down("d")){
                    editor.equip_spawns[editor.current_col_box].x -= 0.1f;
                }
                break;

            case EM_COLLISION:
                if(dungeon.walkable_count > 0) {
                    if(gfc_input_key_down(" ")){
                        if(gfc_input_key_down("w")){
                            dungeon.walkable[editor.current_col_box].y += 0.1f;
                        }

                        if(gfc_input_key_down("s")){
                            dungeon.walkable[editor.current_col_box].y -= 0.1f;
                        }

                        if(gfc_input_key_down("a")){
                            dungeon.walkable[editor.current_col_box].x += 0.1f;
                        }

                        if(gfc_input_key_down("d")){
                            dungeon.walkable[editor.current_col_box].x -= 0.1f;
                        }
                    } else {
                        if(gfc_input_key_down("w")){
                            dungeon.walkable[editor.current_col_box].h += 0.1f;
                        }

                        if(gfc_input_key_down("s")){
                            dungeon.walkable[editor.current_col_box].h -= 0.1f;
                        }

                        if(gfc_input_key_down("a")){
                            dungeon.walkable[editor.current_col_box].w += 0.1f;
                        }

                        if(gfc_input_key_down("d")){
                            dungeon.walkable[editor.current_col_box].w -= 0.1f;
                        }
                    }
                }
                break;
        
        default:
            break;
        }

    }


}

void dungeon_editor_draw(){
    Matrix4 drawBox;
    gf3d_camera_look_at(vector3d(editor.camera_lookat_pos.x, editor.camera_lookat_pos.y - 15, -5), vector3d(editor.camera_lookat_pos.x, editor.camera_lookat_pos.y, -3), vector3d(0,1,0));
    gf3d_model_draw_map(dungeon.floor, dungeon.mat, 256);// vector4d(1,1,1,1), vector4d(1,1,1,1), vector4d(0,0,0,0));

    switch (mode)
    {
    case EM_COLLISION:
        gf2d_font_draw_line_tag("Collision Mode", FT_Normal, gfc_color(1,1,1,1), vector2d(1080, 20));
        break;

    case EM_ENEMY:
        gf2d_font_draw_line_tag("Enemy Mode", FT_Normal, gfc_color(1,1,1,1), vector2d(1080, 20));
        break;

    case EM_GEM:
        gf2d_font_draw_line_tag("Gem Mode", FT_Normal, gfc_color(1,1,1,1), vector2d(1080, 20));
        break;

    case EM_HAZARD:
        gf2d_font_draw_line_tag("Hazard Mode", FT_Normal, gfc_color(1,1,1,1), vector2d(1080, 20));
        break;

    case EM_EQUIP:
        gf2d_font_draw_line_tag("Equip Spawns Mode", FT_Normal, gfc_color(1,1,1,1), vector2d(1080, 20));
        break;
    
    default:
        break;
    }
    
    for (size_t i = 0; i < dungeon.walkable_count; i++){
        int selected = editor.current_col_box != i && mode != EM_COLLISION;
        gfc_matrix_identity(drawBox);
        gfc_matrix_translate(drawBox, vector3d(dungeon.walkable[i].x, dungeon.walkable[i].y, dungeon.walkable[i].z));
        gf3d_model_draw(editor.box_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));

        gfc_matrix_identity(drawBox);
        gfc_matrix_translate(drawBox, vector3d(dungeon.walkable[i].x + dungeon.walkable[i].w, dungeon.walkable[i].y + dungeon.walkable[i].h, dungeon.walkable[i].z + dungeon.walkable[i].d));
        gf3d_model_draw(editor.box_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
    }

    for (size_t i = 0; i < editor.enemy_count; i++){
        int selected = editor.current_col_box != i && mode != EM_ENEMY;
        gfc_matrix_identity(drawBox);
        gfc_matrix_translate(drawBox, vector3d(editor.enemies[i].position.x, editor.enemies[i].position.y, editor.enemies[i].position.z));
        switch (editor.enemies[i].type)
        {
        case Rockman:
            gf3d_model_draw(rockman_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
            break;
        case Skeleton:
            gf3d_model_draw(skeleton_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
            break;
        case Zombie:
            gf3d_model_draw(zombie_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
            break;
         case Slime:
            gf3d_model_draw(slime_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
            break;
        case Goblin:
            gf3d_model_draw(goblin_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
            break;        
        default:
            break;
        }
    }


    for (size_t i = 0; i < editor.hazard_count; i++){
        int selected = editor.current_col_box != i && mode != EM_HAZARD;
        gfc_matrix_identity(drawBox);
        gfc_matrix_translate(drawBox, vector3d(editor.hazards[i].position.x, editor.hazards[i].position.y,editor.hazards[i].position.z));
        switch(editor.hazards[i].type){
            case 0: gf3d_model_draw(spike_hazard_model, drawBox,  vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1)); break;
            case 1: gf3d_model_draw(puddle_hazard_model, drawBox,  vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1)); break;
            case 2: gf3d_model_draw(fire_hazard_model, drawBox,  vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1)); break;
        }
    }

    for (size_t i = 0; i < editor.gem_count; i++){
        int selected = editor.current_col_box != i && mode != EM_GEM;
        gfc_matrix_identity(drawBox);
        gfc_matrix_translate(drawBox, vector3d(editor.gem_spawns[i].x, editor.gem_spawns[i].y, editor.gem_spawns[i].z));
        gf3d_model_draw(gem_model, drawBox, vector4d(1*selected,1,1*selected,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
    }

    for (size_t i = 0; i < editor.equip_count; i++){
        int selected = editor.current_col_box != i && mode != EM_EQUIP;
        gfc_matrix_identity(drawBox);
        gfc_matrix_translate(drawBox, vector3d(editor.equip_spawns[i].x, editor.equip_spawns[i].y, editor.equip_spawns[i].z));
        gf3d_model_draw(editor.box_model, drawBox, vector4d(0,1*selected,0,1), vector4d(1,1,1,1), vector4d(1,1,1,1));
    }

    menu_draw();
}