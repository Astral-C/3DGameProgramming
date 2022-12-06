#ifndef __SHOP_H__
#define __SHOP_H__
#include "entity.h"
#include "gf3d_model.h"

typedef enum {
    LAWYERS,
    COMFORT,
    MONEY_PRINTERS,
    SPEEDY_SERVICE,
    SMILY_SERVICE,
    LUCKY_DAYS,
    DOWNTIME_REDUCER,
    STRONK_JUICE,
    DEFENSE_JUICE,
    ZE_HEALING,
    UPGRADES_MAX
    //more????
} ShopUpgrades;

typedef enum {
    WATER,
    FIRE,
    EARTH,
    DUNGEON_TYPE_MAX
} DungeonType;

extern Vector4D TypeColors[3];

typedef struct {
    int cash;
    int served;
    int fees;
    int fee_timer;
    Model* floor;
    Matrix4 mat;
    uint8_t upgrades[UPGRADES_MAX];
    
    Box collision;
} ShopManager;

typedef struct {
    DungeonType type;
    Model* floor;
    Matrix4 mat;
    Entity* items[15];
    Entity* hazards[15];
    
    int walkable_count;
    Box* walkable;
} DungeonManager;

void world_update();
void world_init();
void world_draw();
void shop_manager_add_cash(int amount);

extern ShopManager shop;

DungeonType GetCurrentDungeonType();

void RandomizeDungeon();

#endif