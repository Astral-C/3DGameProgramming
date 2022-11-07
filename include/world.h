#ifndef __SHOP_H__
#define __SHOP_H__
#include "entity.h"
#include "gf3d_model.h"

typedef enum {
    MINT_TEA_LEAVES,
    INFINITE_SALT_SHAKER,
    SAUL_GOODMAN,
    MONEY_PRINTER,
    //more????
} ResourceType;

typedef enum {
    WATER,
    FIRE,
    EARTH
} DungeonType;

typedef struct {
    int cash;
    int fees;
    int fee_timer;
    Model* floor;
    Matrix4 mat;
} ShopManager;

typedef struct {
    DungeonType type;
    Vector4D color;
    Model* floor;
} DungeonManager;

void world_update();
void world_init();
void world_draw();

#endif