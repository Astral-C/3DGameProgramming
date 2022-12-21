#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "employee.h"
#include "customer.h"
#include "world.h"
#include <stdlib.h>
#include <time.h>
#include "gamestate.h"

#include "menu.h"
#include "loading_screen.h"
#include "dungeon_editor.h"

#include <SDL_mixer.h>

extern int __DEBUG;

GameStates state = MainMenu;
int loaded = 0;
int screentimer = 10;

void game_init(){
    lighting_init();
    load_equipment_textures();
    load_customer_textures();
    entity_system_init(200);
    gf3d_camera_set_scale(vector3d(1,1,1));
    customer_manager_init();
    employee_manager_init();
    Mix_Pause(-1);
    world_init();
    loaded = 1;
}

void play_button(){
    state = LoadingScreen;
}

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    int editor_arg_pos;
    
    srand(time(NULL));

    Sprite *mouse = NULL;
    int mousex,mousey;
    float mouseFrame = 0;
    __DEBUG = 0;
    
    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }

        if (strcmp(argv[a],"--editor") == 0)
        {
            state = Editor;
        }
    }
    
    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(500);
    gfc_audio_init(10, 4, 2, 2, 0, 1);

    mouse = gf2d_sprite_load("images/pointer.png", 32, 32, 16);
    
    slog_sync();

    if(state == MainMenu){
        menu_init(5, "images/titlescreen.png", "audio/menu.wav");
        menu_add_button("Play", gfc_rect(700, 400, 500, 250), gfc_color(0.5,0.8,0.5,1.0), play_button);
        loading_screen_init();
    } else if(state == Editor){
        for (a = 1; a < argc;a++){
            if (strcmp(argv[a],"--editor") == 0){
                dungeon_editor_init(argv[a+1]);
                lighting_init();
            }
        }
    }


    //loading screen ends here

    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;


        switch (state)
        {
        case MainMenu:
            menu_update();
            break;
        
        case Game:
            entity_think_all();
            entity_update_all();

            customer_manager_update();
            employee_manager_update();
            world_update();
            break;

        case Editor:
            dungeon_editor_update();
            break;

        // Yes the loading screen is fake. No, I don't care right now.
        // Hey its me 20 minutes later, I misread landing as loading. Fuck.
        case LoadingScreen:
            if(!loaded){
                game_init();
            } else {
                screentimer--;
            }

        default:
            break;
        }


        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());


        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        gf3d_vgraphics_render_start();

            //3D draws
            switch (state)
                {
                case MainMenu:
                    menu_draw();
                    break;
                
                case Game:
                    world_draw();
                    entity_draw_all_sorted();
                    employee_manager_draw();
                    break;

                case LoadingScreen:
                    loading_screen_draw();

                    if(screentimer <= 0) state = Game;
                    break;

                case Editor:
                    dungeon_editor_draw();
                    break;
                    
                default:
                    break;
                }
            //2D draws
                
                gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);
        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
