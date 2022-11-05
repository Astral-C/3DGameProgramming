#include "employee.h"
#include "gf3d_camera.h"
#include "gfc_input.h"
#include "gf2d_font.h"
#include "gfc_text.h"
#include <math.h>

static EmployeeManager Employees = {0};


void employee_manager_init(){
    for (size_t i = 0; i < EMPLOYEE_MAX; i++){
        Employees.employee_slots[i].entity = spawn_employee();
        generate_employee_data(&Employees.employee_slots[i]); // generate a random employee
        Employees.employee_slots[i].entity->customData = &Employees.employee_slots[i];
    }
    
    Employees.focused_idx = 0;
    Employees.focused = Employees.employee_slots[0].entity;
}


void employee_manager_update(){
    //todo

    if(gfc_input_key_pressed(" ")){
        Employees.focused = Employees.employee_slots[Employees.focused_idx++ % EMPLOYEE_MAX].entity;
    }

    gf3d_camera_look_at(vector3d(Employees.focused->position.x, Employees.focused->position.y - 25, -6), vector3d(Employees.focused->position.x, Employees.focused->position.y, 0), vector3d(0,1,0));
}


void generate_employee_data(EmployeeData* data){
    //todo
    data->move_timer = 700;
    data->type = rand() % EMPLOYEE_TYPES;
    data->attack = rand() % 255;
    data->defense = rand() % 255;
    data->health = rand() % 255;
    data->speed = rand() % 255;
}

void employee_think(Entity* self){
    if(self != Employees.focused){
        if(((EmployeeData*)self->customData)->move_timer == 0){
            self->velocity.x = fmod((float)drand48(), 0.08) - 0.04;
            self->velocity.y = fmod((float)drand48(), 0.08) - 0.04;
            ((EmployeeData*)self->customData)->move_timer = 700;
            //self->velocity.y = 0.005;
        } else {
            ((EmployeeData*)self->customData)->move_timer--;
        }

        if((self->position.x > 35 && self->velocity.x > 0) || (self->position.x < -35 && self->velocity.x < 0)){
            self->velocity.x = -self->velocity.x;
        }

        if(self->position.y > 50 || self->position.y < -50) {
            self->velocity.y = -self->velocity.y;
        }
    } else {
        if(gfc_input_key_down("d") && self->position.x > -35){
            self->velocity.x = -0.1;
        } else if(gfc_input_key_down("a") && self->position.x < 35){
            self->velocity.x = 0.1;
        } else {
            self->velocity.x = 0;
        }

        if(gfc_input_key_down("s") && self->position.y > -50){
            self->velocity.y = -0.1;
        } else if(gfc_input_key_down("w") && self->position.y < 50){
            self->velocity.y = 0.1;
        } else {
            self->velocity.y = 0;
        }
    }
}

void employee_manager_draw(){
    gf2d_draw_rect(gfc_rect((1280 / 2) - 120, (720 / 2) - 270, 300, 300),gfc_color8(255,255,255,255));
    gf2d_font_draw_line_tag("Employee",FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 250));

    EmployeeData* data = ((EmployeeData*)Employees.focused->customData);

    TextWord line;

    switch (data->type)
    {
    case Goblin:
        gf2d_font_draw_line_tag("Goblin",FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 230));
        break;

    case Knight:
        gf2d_font_draw_line_tag("Knight",FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 230));
        break;
    
    case Ghost:
        gf2d_font_draw_line_tag("Ghost",FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 230));
        break;

    case Elf:
        gf2d_font_draw_line_tag("Elf",FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 230));
        break;

    case CatPerson:
        gf2d_font_draw_line_tag("CatPerson",FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 230));
        break;
    
    default:
        gf2d_font_draw_line_tag("???",FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 230));
        break;
    }

    snprintf(line, sizeof(TextWord), "Health: %d", data->health);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 210));
    
    snprintf(line, sizeof(TextWord), "Attack: %d", data->attack);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 190));

    snprintf(line, sizeof(TextWord), "Defense: %d", data->defense);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 170));

    snprintf(line, sizeof(TextWord), "Speed: %d", data->speed);
    gf2d_font_draw_line_tag(line,FT_H1,gfc_color(1,1,1,1), vector2d((1280 / 2) - 80, (720 / 2) - 150));

}

Entity* spawn_employee(){
    Entity* employee_ent = entity_new();

    employee_ent->model = gf3d_model_load_full("models/cube.obj", "images/cube.png");
    employee_ent->position = vector3d(0,0,0);
    employee_ent->think = employee_think;

    employee_ent->velocity.x = fmod((float)drand48(), 0.08) - 0.04;
    employee_ent->velocity.y = fmod((float)drand48(), 0.08) - 0.04;

    return employee_ent;
}