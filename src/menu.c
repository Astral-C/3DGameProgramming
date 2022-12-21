#include "menu.h"
#include "simple_logger.h"

Menu menu = {0};
static int pressed_last_frame = 0;

void menu_cleanup(){
    if(menu.button_image) gf2d_sprite_free(menu.button_image);
    if(menu.background_image) gf2d_sprite_free(menu.background_image);
}

void menu_init(int max_buttons, char* background_image, char* music){
    menu.button_count = max_buttons;
    menu.buttons = malloc(sizeof(MenuButton) * max_buttons);
    memset(menu.buttons, 0, sizeof(MenuButton) * max_buttons);

    if(background_image != NULL){
        menu.background_image = gf2d_sprite_load_image(background_image);
    }

    if(music != NULL){
        menu.music = gfc_sound_load(music, 1.0, 0);
    }
    
    if(menu.music != NULL){
        gfc_sound_play(menu.music, 1, 0.8f, -1, -1);
    }

    menu.button_image = gf2d_sprite_load_image("images/button.png");

    atexit(menu_cleanup);
}

void menu_add_button(TextLine text, Rect pos, Color color, void (*click)()){
    for (int i = 0; i < menu.button_count; i++){
        if(!menu.buttons[i]._inuse){
            menu.buttons[i]._inuse = 1;
            menu.buttons[i].onClick = click;
            menu.buttons[i].bounds = pos;
            menu.buttons[i].bounds.w = menu.button_image->frameWidth;
            menu.buttons[i].bounds.h = menu.button_image->frameHeight;
            menu.buttons[i].button_color = color;
            strncpy(menu.buttons[i].text, text, sizeof(TextLine));

            break;
        }
    }
}

void menu_clear_buttons(){
    memset(menu.buttons, 0, sizeof(MenuButton) * menu.button_count);
}

void set_background_image(char* background_image){
    if(menu.background_image != NULL) gf2d_sprite_free(menu.background_image);

    menu.background_image = gf2d_sprite_load_image(background_image);
}

void menu_stop_music(){
    gfc_sound_free(menu.music);
}

void menu_update(){
    int x, y;
    Uint32 mouse = SDL_GetMouseState(&x, &y);

    if(mouse & SDL_BUTTON(SDL_BUTTON_LEFT) && pressed_last_frame == 0){
        for(int i = 0; i < menu.button_count; i++){
            if(menu.buttons[i]._inuse && gfc_point_in_rect(vector2d(x,y), menu.buttons[i].bounds)){
                menu.buttons[i].onClick();
            }
        }
        pressed_last_frame = 1;
    } else {
        pressed_last_frame = mouse & SDL_BUTTON(SDL_BUTTON_LEFT);
    }
    
}

void menu_draw(){
    if(menu.background_image) gf2d_sprite_draw(menu.background_image, vector2d(0,0), vector2d(2,2), vector3d(0,0,0), gfc_color(1,1,1,1), 0);
    for(int i = 0; i < menu.button_count; i++){
        if(menu.buttons[i]._inuse) {
            gf2d_sprite_draw(menu.button_image, vector2d(menu.buttons[i].bounds.x, menu.buttons[i].bounds.y), vector2d(2,2), vector3d(0,0,0), gfc_color(1,1,1,1), 0);
            Vector2D text_size = gf2d_font_get_bounds_tag(menu.buttons[i].text, FT_H1);
            gf2d_font_draw_line_tag(menu.buttons[i].text, FT_H1, gfc_color(1,1,1,1), vector2d(menu.buttons[i].bounds.x + ((menu.buttons[i].bounds.w / 2) - (text_size.x / 4)), menu.buttons[i].bounds.y + ((menu.buttons[i].bounds.h / 2) - (text_size.y / 4))));
        }
    }
}