#ifndef __SHOP_H__
#define __SHOP_H__
#include "entity.h"
#include "gf3d_model.h"

typedef enum {
    MINT_TEA_LEAVES,
    //more????
} ResourceType;

typedef struct {
    int cash;
    Model* walls;
    Model* molding;
    Model* floor;
    Matrix4 mat;
} ShopManager;

void shop_update();
void shop_init();

#endif