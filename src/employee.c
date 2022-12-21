#include "employee.h"
#include "customer.h"
#include "world.h"
#include "gf3d_camera.h"
#include "gfc_input.h"
#include "gf2d_font.h"
#include "gfc_text.h"
#include <math.h>
#include "sg_util.h"

EmployeeManager Employees = {0};
static DungeonType CurrentDungeonType;
Vector3D CamFocus, PrevFocus;
uint8_t FocusTimer, FocusSwitch;

void employee_manager_init(){
    for (size_t i = 0; i < EMPLOYEE_MAX; i++){
        generate_employee_data(&Employees.employee_slots[i]); // generate a random employee
        Employees.employee_slots[i].entity = spawn_employee(Employees.employee_slots[i].type);
        Employees.employee_slots[i].entity->customData = &Employees.employee_slots[i];
    }
    
    Employees.focused_idx = 0;
    Employees.focused = Employees.employee_slots[0].entity;
    Employees.passive_effect_timer = 600;
    CamFocus = Employees.focused->position;
    PrevFocus = Employees.focused->position;
    FocusTimer = 10;
    FocusSwitch = 0;
}

void employee_manager_update(){
    //todo
    Employees.passive_effect_timer--;
    if(Employees.passive_effect_timer <= 0){
        if(((EmployeeData*)Employees.focused->customData)->health < 100){
            ((EmployeeData*)Employees.focused->customData)->health += 1 * shop.upgrades[ZE_HEALING];
        }
        Employees.passive_effect_timer = 600;
    }

    if(gfc_input_key_pressed(" ")){
        if(((EmployeeData*)Employees.focused->customData)->in_dungeon){
            ((EmployeeData*)Employees.focused->customData)->in_dungeon = 0;
            Employees.focused->position = vector3d(0,0,0);
        }
        Employees.focused = Employees.employee_slots[Employees.focused_idx++ % EMPLOYEE_MAX].entity;
        PrevFocus = CamFocus;
        FocusSwitch = 1;
        FocusTimer = 0;
    }


    if(FocusSwitch){
        FocusTimer++;
    }

    CamFocus = lerpv(PrevFocus, Employees.focused->position, (float)FocusTimer/10.0f);

    if(FocusTimer == 10){
        FocusSwitch = 0;
    }

    gf3d_camera_look_at(vector3d(CamFocus.x, CamFocus.y - 15, -5), vector3d(CamFocus.x, CamFocus.y, -3), vector3d(0,1,0));

    for(int i = 0; i < EMPLOYEE_MAX; i++){
        if(Employees.employee_slots[i].entity == NULL){
            //todo
        }
    }
}


void generate_employee_data(EmployeeData* data){
    //todo
    data->move_timer = 200;
    data->type = rand() % EMPLOYEE_TYPES;
    data->attack = rand() % 255;
    data->defense = rand() % 255;
    data->health = (rand() % 90) + 10;
    data->speed = rand() % 255;
}

void employee_think(Entity* self){
    if(self != Employees.focused){
        if(((EmployeeData*)self->customData)->target != NULL){
            self->velocity.x = (((EmployeeData*)self->customData)->target->position.x - self->position.x) * 0.01 * (shop.upgrades[SPEEDY_SERVICE] + 1);
            self->velocity.y = (((EmployeeData*)self->customData)->target->position.y - self->position.y) * 0.01 * (shop.upgrades[SPEEDY_SERVICE] + 1);
            if(vector3d_magnitude_between(self->position, ((EmployeeData*)self->customData)->target->position) < 7){
                CustomerData* customer = (CustomerData*)((EmployeeData*)self->customData)->target->customData;
                entity_free(customer->entity);

                customer->entity = NULL;
                customer->targeter = NULL;
                customer->want_type = EMPLOYEE_TYPES;

                shop_manager_add_cash(customer->payout);

                ((EmployeeData*)self->customData)->target = NULL;
            }
        } else {    
            ((EmployeeData*)self->customData)->move_timer--;
            if(((EmployeeData*)self->customData)->move_timer <= 0){
                self->velocity.x = fmod((float)drand48(), 0.095) - 0.0475;
                self->velocity.y = fmod((float)drand48(), 0.095) - 0.0475;
                ((EmployeeData*)self->customData)->move_timer = 300 - (10 * shop.upgrades[DOWNTIME_REDUCER]);
                //check if any customers are looking for us
                if(((EmployeeData*)self->customData)->target == NULL){
                    for (size_t i = 0; i < CUSTOMER_MAX; i++){
                        if(Customers.customer_slots[i].targeter == NULL && Customers.customer_slots[i].entity != NULL){
                            ((EmployeeData*)self->customData)->target = Customers.customer_slots[i].entity;
                            Customers.customer_slots[i].targeter = self;
                            self->velocity.x = (((EmployeeData*)self->customData)->target->position.x - self->position.x) * 0.005 * (shop.upgrades[SPEEDY_SERVICE] + 1);
                            self->velocity.y = (((EmployeeData*)self->customData)->target->position.y - self->position.y) * 0.005 * (shop.upgrades[SPEEDY_SERVICE] + 1);
                            break;
                        }
                    }
                }
                //self->velocity.y = 0.005;
            }
        }
        
        if(!gfc_point_in_box(self->position, shop.collision)){
            self->velocity.x = fmod((float)drand48(), 0.095) - 0.0475;
            self->velocity.y = fmod((float)drand48(), 0.095) - 0.0475;
        }
    } else {
        if(gfc_input_key_released("f")){
            ((EmployeeData*)self->customData)->in_dungeon = !((EmployeeData*)self->customData)->in_dungeon;
            if(((EmployeeData*)self->customData)->in_dungeon){
                RandomizeDungeon();
                CurrentDungeonType = GetCurrentDungeonType();
                ((EmployeeData*)self->customData)->move_timer = 100;
                self->position = vector3d(300, 0, 0);
            } else {
                self->position = vector3d(0, 0, 0);
            }

        }

        if(gfc_input_key_down("s")){
            self->velocity.y = -0.1;
        } else if(gfc_input_key_down("w")){
            self->velocity.y = 0.1;
        } else {
            self->velocity.y = 0;
        }

        if(gfc_input_key_down("d")){
        self->velocity.x = -0.1;
        } else if(gfc_input_key_down("a")){
            self->velocity.x = 0.1;
        } else {
            self->velocity.x = 0;
        }

        if(((EmployeeData*)self->customData)->in_dungeon){
            switch (((EmployeeData*)self->customData)->type){
                case Knight:
                case CatPerson:
                    if(CurrentDungeonType == FIRE && ((EmployeeData*)self->customData)->move_timer == 0){
                        ((EmployeeData*)self->customData)->health--;
                        ((EmployeeData*)self->customData)->move_timer = 250;
                    } else {
                        ((EmployeeData*)self->customData)->move_timer--;
                    }
                    break;

                case Ghost:
                case Elf:
                    if(CurrentDungeonType == EARTH && ((EmployeeData*)self->customData)->move_timer == 0){
                        if(((EmployeeData*)self->customData)->health < 100) ((EmployeeData*)self->customData)->health++;
                        ((EmployeeData*)self->customData)->move_timer = 100;
                    } else {
                        ((EmployeeData*)self->customData)->move_timer--;
                    }
                    break;
                default:
                    break;
            }

        }
    }
}

void employee_update(Entity* self){
    if(self != Employees.focused) return;

    Vector3D pos_step;
    vector3d_add(pos_step, self->position, self->velocity);
    if(!Employees.employee_slots[Employees.focused_idx].in_dungeon){
        if(!gfc_point_in_box(pos_step, shop.collision)){
            self->velocity.x = 0;
            self->velocity.y = 0;
        }
    } else {
        int in_box = 0; //assume we are never in a bounding box
        for (size_t i = 0; i < dungeon.walkable_count; i++){
            if(gfc_point_in_box(pos_step, dungeon.walkable[i])){
                in_box = 1;
                break;
            }
        }
        if(!in_box){
            self->velocity.x = 0;
            self->velocity.y = 0; 
        }
        
    }
}

void employee_manager_draw(){
    gf2d_font_draw_line_tag("Employee",FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 250));

    EmployeeData* data = ((EmployeeData*)Employees.focused->customData);

    TextWord line;

    switch (data->type)
    {
    case Witch:
        gf2d_font_draw_line_tag("Witch",FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 230));
        break;

    case Knight:
        gf2d_font_draw_line_tag("Knight",FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 230));
        break;
    
    case Ghost:
        gf2d_font_draw_line_tag("Ghost",FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 230));
        break;

    case Elf:
        gf2d_font_draw_line_tag("Elf",FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 230));
        break;

    case CatPerson:
        gf2d_font_draw_line_tag("CatPerson",FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 230));
        break;
    
    default:
        gf2d_font_draw_line_tag("???",FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 230));
        break;
    }

    snprintf(line, sizeof(TextWord), "Health: %d", data->health);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 210));
    
    snprintf(line, sizeof(TextWord), "Attack: %d", data->attack + (2 * shop.upgrades[STRONK_JUICE]));
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 190));

    snprintf(line, sizeof(TextWord), "Defense: %d", data->defense + (2 * shop.upgrades[DEFENSE_JUICE]));
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 170));

    snprintf(line, sizeof(TextWord), "Speed: %d", data->speed);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,0.3,0.3,1), vector2d(25, (720 / 2) - 150));

    gf2d_font_draw_line_tag("Equipment", FT_H1,gfc_color(1,0.3,0.3,1), vector2d(120, (720 / 2) - 250));

    snprintf(line, sizeof(TextWord), Equipment[data->equipment[EQUIP_HEAD]].name);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,0.3,0.3,1), vector2d(120, (720 / 2) - 230));

    snprintf(line, sizeof(TextWord), Equipment[data->equipment[EQUIP_TORSO]].name);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,0.3,0.3,1), vector2d(120, (720 / 2) - 210));

    snprintf(line, sizeof(TextWord), Equipment[data->equipment[EQUIP_LEGS]].name);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,0.3,0.3,1), vector2d(120, (720 / 2) - 190));


    if(data->in_dungeon){
        switch (GetCurrentDungeonType())
        {
        case FIRE:
            snprintf(line, sizeof(TextWord), "Fire Dungeon");
            break;
        case WATER:
            snprintf(line, sizeof(TextWord), "Water Dungeon");
            break;
        case EARTH:
            snprintf(line, sizeof(TextWord), "Earth Dungeon");
            break;
        default:
            break;
        }
        gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,1,1,1), vector2d(25, 20));
    }

}

void employee_die(Entity* self){
    //todo!
}

Entity* spawn_employee(EmployeeType type){
    Entity* employee_ent = entity_new();


    switch (type)
    {
    case Witch:
        employee_ent->model = gf3d_model_load_full("models/cube.obj", "images/witch.png");
        break;

    case Knight:
        employee_ent->model = gf3d_model_load_full("models/cube.obj", "images/knight.png");
        break;
    
    case Ghost:
        employee_ent->model = gf3d_model_load_full("models/cube.obj", "images/ghost.png");
        break;

    case Elf:
        employee_ent->model = gf3d_model_load_full("models/cube.obj", "images/elf.png");
        break;

    case CatPerson:
        employee_ent->model = gf3d_model_load_full("models/cube.obj", "images/catperson.png");
        break;
    
    default:
        employee_ent->model = gf3d_model_load_full("models/cube.obj", "images/cube.png");
        break;
    }

    employee_ent->position = vector3d(0,0,0);
    employee_ent->think = employee_think;
    employee_ent->onDeath = employee_die;
    employee_ent->update = employee_update;

    employee_ent->velocity.x = fmod((float)drand48(), 0.095) - 0.0475;
    employee_ent->velocity.y = fmod((float)drand48(), 0.095) - 0.0475;
    

    return employee_ent;
}