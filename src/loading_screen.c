#include "loading_screen.h"
#include "gf2d_draw.h"
#include "gf2d_sprite.h"
#include "gf3d_model.h"
#include <simple_json.h>

Sprite* loading_screen_graphic = NULL;

void loading_screen_end(){
    if(loading_screen_graphic) gf2d_sprite_free(loading_screen_graphic);
}

void loading_screen_init(){
    loading_screen_graphic = gf2d_sprite_load_image("images/loading.png");
    atexit(loading_screen_end);
}

void loading_screen_draw(){
    gf2d_sprite_draw(loading_screen_graphic, vector2d(0,0), vector2d(2,2), vector3d(0,0,0), gfc_color(1,1,1,1), 0);
}