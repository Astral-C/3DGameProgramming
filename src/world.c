#include "world.h"
#include "gfc_audio.h"
#include "employee.h"
#include "equipment.h"
#include "gf2d_font.h"
#include "gfc_input.h"
#include "hazard.h"

ShopManager shop = {0};
static DungeonManager dungeon = {0};

Vector4D TypeColors[3] = {{0.5, 0.65, 0.9, 1.0}, {1.0, 0.65, 0.75, 1.0}, {0.8, 0.75, 0.4, 1.0}};
Sound* bg_music_shop = NULL;

void world_init(){
    //todo
    //shop.walls = gf3d_model_load("walls");
    shop.floor = gf3d_model_load_full("models/shop.obj", "images/ShopAlbedo.png");
    dungeon.floor = gf3d_model_load_full("models/floor.obj", "images/ground_03.png");
    //shop.molding = gf3d_model_load("floor2");
    gfc_matrix_identity(shop.mat);
    gfc_matrix_identity(dungeon.mat);
    gfc_matrix_translate(dungeon.mat, vector3d(300, 0, 0));
    shop.cash = 10000;
    shop.fee_timer = 500;
    shop.fees = 150;

    spawn_spike();

    bg_music_shop = gfc_sound_load("audio/Caketown1.wav", 0.8, 0);
    gfc_sound_play(bg_music_shop, 1, 0.8, -1, -1);
}

void world_draw(){
    //gf3d_model_draw(shop.walls,shop.mat);
    gf3d_model_draw(shop.floor,shop.mat, vector4d(1,1,1,1), vector4d(1,1,1,1));
    gf3d_model_draw(dungeon.floor, dungeon.mat, TypeColors[dungeon.type], vector4d(1,1,1,1));
    
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

    for (size_t i = 0; i < 15; i++){
        if(dungeon.items[i] != NULL){
            entity_free(dungeon.items[i]);
        }

        dungeon.items[i] = spawn_random_equipment();
    }

    Entity* cash;
    for (size_t i = 0; i < (rand() % 10) + (rand() % (shop.upgrades[LUCKY_DAYS] + 1)); i++){
        cash = entity_new();
        cash->model = gf3d_model_load_full("models/gem.obj", "images/gem.png");
        cash->position = vector3d(((rand() % 70) - 35) + 300, (rand() % 100) - 50, 0);
        cash->think = cash_think;
    
    }
    

}

void shop_manager_add_cash(int amount){
    shop.cash += amount + (5 * shop.upgrades[SMILY_SERVICE]);
    shop.served++;
    if(shop.served % 10 == 0){
        shop.fees += 4;
    }
}