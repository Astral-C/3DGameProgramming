#ifndef __MENU_H__
#define __MENU_H__
#include "gfc_color.h"
#include "gf2d_draw.h"
#include "gfc_audio.h"
#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gfc_types.h"

typedef struct {
    int _inuse;
    Rect bounds;
    Color button_color;
    TextLine text;

    void (*onClick)();
} MenuButton;

typedef struct {
    int button_count;
    MenuButton* buttons;
    Sound* music;
    Sprite* button_image;
    Sprite* background_image;
} Menu;

void menu_init(int max_buttons, char* background_image, char* music);

void menu_add_button(TextLine text, Rect pos, Color color, void (*click)());
void menu_clear_buttons();
void set_background_image(char* background_image);

void menu_update();
void menu_draw();

#endif