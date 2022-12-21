#include "world.h"
#include "gfc_audio.h"
#include "employee.h"
#include "equipment.h"
#include "gf2d_font.h"
#include "gfc_input.h"
#include "hazard.h"
#include "gf2d_draw.h"
#include "dungeon_editor.h"

ShopManager shop = {0};
DungeonManager dungeon = {0};

Vector4D TypeColors[3] = {{0.5, 0.65, 0.9, 1.0}, {1.0, 0.65, 0.75, 1.0}, {0.8, 0.75, 0.4, 1.0}};
Sound* bg_music_shop = NULL;
Model* box = NULL;

void furniture_think(Entity* self){
    if(Employees.employee_slots[Employees.focused_idx].in_dungeon && !self->hidden){
        self->hidden = 1;
    } else if(!Employees.employee_slots[Employees.focused_idx].in_dungeon && self->hidden){
        self->hidden = 0;
    }
}

void spawn_furniture(){
    int price = rand() % 300;
    if(shop.cash > price);
    float percent_x = (float)(rand() % 100) / 100.0f;
    float percent_y = (float)(rand() % 100) / 100.0f;
    Entity* furniture = entity_new_at(vector3d((shop.collision.x + 0.5f + ((shop.collision.w-0.5f) * percent_x)), (shop.collision.y + 0.5f + ((shop.collision.h - 0.5f) * percent_y)), 0));
    shop.furniture_count += 1;
    shop.cash -= price;
    furniture->think = furniture_think;

    int type = rand() % 5;

    switch (type)
    {
    case 0:
        furniture->model = gf3d_model_load_full("models/cube.obj", "images/big_plant.png");
        break;
    case 1:
        furniture->model = gf3d_model_load_full("models/cube.obj", "images/flag_furniture.png");
        break;
    case 2:
        furniture->model = gf3d_model_load_full("models/cube.obj", "images/lamp_furniture.png");
        break;
    case 3:
        furniture->model = gf3d_model_load_full("models/cube.obj", "images/plant_furniture.png");
        break;
    case 4:
        furniture->model = gf3d_model_load_full("models/cube.obj", "images/vending_machine.png");
        break;
    }
}

void world_init(){
    //todo
    //shop.walls = gf3d_model_load("walls");
    shop.floor = gf3d_model_load_full("models/shop.obj", "images/shoptex.png");
    box = gf3d_model_load_full("models/point.obj", "images/default.png");
    //shop.molding = gf3d_model_load("floor2");
    gfc_matrix_identity(shop.mat);
    gfc_matrix_identity(dungeon.mat);
    gfc_matrix_translate(dungeon.mat, vector3d(300, 0, 0));
    shop.cash = 10000;
    shop.fee_timer = 500;
    shop.fees = 150;

    bg_music_shop = gfc_sound_load("audio/Caketown1.wav", 0.8, 0);
    gfc_sound_play(bg_music_shop, 1, 0.8, -1, -1);

    shop.collision = gfc_box(-30, -30, -10, 60, 60, 10);
}

void world_draw(){
    //gf3d_model_draw(shop.walls,shop.mat);

    if(!Employees.employee_slots[Employees.focused_idx].in_dungeon){
        gf3d_model_draw_map(shop.floor,shop.mat,256);
    } else {
        gf3d_model_draw_map(dungeon.floor,shop.mat,256);
    }
    //gf3d_model_draw(dungeon.floor, dungeon.mat, TypeColors[dungeon.type], vector4d(1,1,1,1),vector4d(0,0,0,0));


    char money[32];
    snprintf(money, sizeof(money), "Shop Money: $%d", shop.cash);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(25, 45));

    snprintf(money, sizeof(money), "Customers Served: %d", shop.served);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(25, 65));

    snprintf(money, sizeof(money), "Lawyer Lvl %d", shop.upgrades[0]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 65));
    snprintf(money, sizeof(money), "Comfort Lvl %d", shop.upgrades[1]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 85));
    snprintf(money, sizeof(money), "Money Printer Lvl %d", shop.upgrades[2]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 105));
    snprintf(money, sizeof(money), "Speedy Service Lvl %d", shop.upgrades[3]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 125));
    snprintf(money, sizeof(money), "Smily Service Lvl %d", shop.upgrades[4]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 145));
    snprintf(money, sizeof(money), "Lucky Days Lvl %d", shop.upgrades[5]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 165));
    snprintf(money, sizeof(money), "Downtime Reducer Lvl %d", shop.upgrades[6]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 185));
    snprintf(money, sizeof(money), "Stronk Juice Lvl %d", shop.upgrades[7]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 205));
    snprintf(money, sizeof(money), "Defense Juice Lvl %d", shop.upgrades[8]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 225));
    snprintf(money, sizeof(money), "Passive Heading Lvl %d", shop.upgrades[9]);
    gf2d_font_draw_line_tag(money,FT_H1,gfc_color(1,1,1,1), vector2d(1280-275, 245));
}

void world_update(){
    //todo: drop items?
    if(shop.fee_timer == 0){
        shop.cash -= (shop.fees - (shop.upgrades[LAWYERS] << 1));
        shop.fee_timer = 500;
    }

    shop.fee_timer--;

    if(gfc_input_key_pressed("1") && shop.cash > 100 * shop.upgrades[0]){
        shop.upgrades[0]++;
        shop.cash -= 100 * shop.upgrades[0];
    }

    if(gfc_input_key_pressed("2") && shop.cash > 100 * shop.upgrades[1]){
        shop.upgrades[1]++;
        shop.cash -= 100 * shop.upgrades[1];
    }

    if(gfc_input_key_pressed("3") && shop.cash > 100 * shop.upgrades[2]){
        shop.upgrades[2]++;
        shop.cash -= 100 * shop.upgrades[2];
    }

    if(gfc_input_key_pressed("4") && shop.cash > 100 * shop.upgrades[3]){
        shop.upgrades[3]++;
        shop.cash -= 100 * shop.upgrades[3];
    }

    if(gfc_input_key_pressed("5") && shop.cash > 100 * shop.upgrades[4]){
        shop.upgrades[4]++;
        shop.cash -= 100 * shop.upgrades[4];
    }

    if(gfc_input_key_pressed("6") && shop.cash > 100 * shop.upgrades[5]){
        shop.upgrades[5]++;
        shop.cash -= 100 * shop.upgrades[5];
    }

    if(gfc_input_key_pressed("7") && shop.cash > 100 * shop.upgrades[6]){
        shop.upgrades[6]++;
        shop.cash -= 100 * shop.upgrades[6];
    }

    if(gfc_input_key_pressed("8") && shop.cash > 100 * shop.upgrades[7]){
        shop.upgrades[7]++;
        shop.cash -= 100 * shop.upgrades[7];
    }

    if(gfc_input_key_pressed("9") && shop.cash > 100 * shop.upgrades[8]){
        shop.upgrades[8]++;
        shop.cash -= 100 * shop.upgrades[8];
    }

    if(gfc_input_key_pressed("0") && shop.cash > 100 * shop.upgrades[9]){
        shop.upgrades[9]++;
        shop.cash -= 100 * shop.upgrades[9];
    }

    if(gfc_input_key_pressed("p")){
        spawn_furniture();
    }

}

DungeonType GetCurrentDungeonType(){
    return dungeon.type;
}

void cash_think(Entity* self){
    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
        return;
    } 

    self->rotation.z += 0.02;

    if(vector3d_magnitude_between(self->position, Employees.focused->position) < 2){
        shop.cash += 100 + (shop.upgrades[MONEY_PRINTERS] * 10);
        entity_free(self);
    }

}

void RandomizeDungeon(){
    dungeon.type = rand() % DUNGEON_TYPE_MAX;

    SJson* config = NULL;
    switch (dungeon.type)
    {
    case WATER:
        config = sj_load("config/water_dungeon.json");
        break;
    case FIRE:
        config = sj_load("config/fire_dungeon.json");
        break;
    case EARTH:
        config = sj_load("config/earth_dungeon.json");
        break;
    default:
        break;
    }

    if(config == NULL){
        ((EmployeeData*)Employees.focused->customData)->in_dungeon = 0; //what
    }

    char* model_path = sj_get_string_value(sj_object_get_value(config, "model"));
    char* texture_path = sj_get_string_value(sj_object_get_value(config, "texture"));

    gfc_matrix_identity(dungeon.mat);
    dungeon.floor = gf3d_model_load_full(model_path, texture_path);

    SJson* boxes = sj_object_get_value(config, "collision");
    SJson* enemy_spawns = sj_object_get_value(config, "enemy_spawns");
    SJson* hazard_spawns = sj_object_get_value(config, "hazard_spawns");
    SJson* gem_spawns = sj_object_get_value(config, "gem_spawns");
    SJson* equip_spawns = sj_object_get_value(config, "equip_spawns");
    SJson* player_spawn = sj_object_get_value(config, "player_spawn");

    if(boxes){
        dungeon.walkable_count = sj_array_get_count(boxes);
        dungeon.walkable = realloc(dungeon.walkable, sizeof(Box)*dungeon.walkable_count);
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
        int enemy_count = sj_array_get_count(enemy_spawns);
        for (size_t i = 0; i < enemy_count; i++){
            SJson* current = sj_array_get_nth(enemy_spawns, i);
            EnemyType type;
            Vector3D pos;
            sj_get_integer_value(sj_object_get_value(current, "type"), (int*)&type);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 0), &pos.x);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 1), &pos.y);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 2), &pos.z);

            spawn_enemy(type, pos);
        }
    }
    
    if(hazard_spawns){
        int hazard_count = sj_array_get_count(hazard_spawns);
        for (size_t i = 0; i < hazard_count; i++){
            SJson* current = sj_array_get_nth(hazard_spawns, i);
            int type;
            Vector3D pos;
            sj_get_integer_value(sj_object_get_value(current, "type"), (int*)&type);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 0), &pos.x);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 1), &pos.y);
            sj_get_float_value(sj_array_get_nth(sj_object_get_value(current, "position"), 2), &pos.z);

            switch (type)
            {
                case 0: spawn_puddle(pos); break;
                case 1: spawn_spike(pos); break;
                case 2: spawn_fire(pos); break;
                default: break;
            }
        }
    }

    if(gem_spawns){
        int gem_count = sj_array_get_count(gem_spawns);
        for (size_t i = 0; i < gem_count; i++){
            SJson* current = sj_array_get_nth(gem_spawns, i);

            Vector3D pos;
            sj_get_float_value(sj_array_get_nth(current, 0), &pos.x);
            sj_get_float_value(sj_array_get_nth(current, 1), &pos.y);
            sj_get_float_value(sj_array_get_nth(current, 2), &pos.z);
            Entity* cash = entity_new_at(pos);
            cash->model = gf3d_model_load_full("models/gem.obj", "images/gem.png");
            cash->think = cash_think;
        }
    }

    if(equip_spawns){
        int equip_count = sj_array_get_count(equip_spawns);
        for (size_t i = 0; i < equip_count; i++){
            SJson* current = sj_array_get_nth(equip_spawns, i);

            Vector3D pos;
            sj_get_float_value(sj_array_get_nth(current, 0), &pos.x);
            sj_get_float_value(sj_array_get_nth(current, 1), &pos.y);
            sj_get_float_value(sj_array_get_nth(current, 2), &pos.z);
            spawn_random_equipment(pos);
        }
    }
    

}

void shop_manager_add_cash(int amount){
    shop.cash += amount + (5 * shop.upgrades[SMILY_SERVICE]);
    shop.served++;
    if(shop.served % 10 == 0){
        shop.fees += 4;
    }
}