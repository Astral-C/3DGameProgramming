#include "shop.h"

static ShopManager shop = {0};

void shop_init(){
    //todo
    //shop.walls = gf3d_model_load("walls");
    shop.floor = gf3d_model_load("floor");
    //shop.molding = gf3d_model_load("floor2");
    gfc_matrix_identity(shop.mat);
}

void shop_draw(){
    //gf3d_model_draw(shop.walls,shop.mat);
    gf3d_model_draw(shop.floor,shop.mat, vector4d(1,1,1,0), vector4d(1,1,1,0));
    //gf3d_model_draw(shop.molding,shop.mat);
}