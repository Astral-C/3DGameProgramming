#include "employee.h"
#include "gf3d_camera.h"
#include "gfc_input.h"
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
}

void employee_think(Entity* self){
    if(self != Employees.focused){
        if(((EmployeeData*)self->customData)->move_timer == 0){
            self->velocity.x = fmod((float)drand48(), 0.04) - 0.02;
            self->velocity.y = fmod((float)drand48(), 0.04) - 0.02;
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
            self->velocity.x = -0.05;
        } else if(gfc_input_key_down("a") && self->position.x < 35){
            self->velocity.x = 0.05;
        } else {
            self->velocity.x = 0;
        }

        if(gfc_input_key_down("s") && self->position.x < 50){
            self->velocity.y = -0.05;
        } else if(gfc_input_key_down("w") && self->position.x > -50){
            self->velocity.y = 0.05;
        } else {
            self->velocity.y = 0;
        }
    }
}

Entity* spawn_employee(){
    Entity* employee_ent = entity_new();

    employee_ent->model = gf3d_model_load("cube");
    employee_ent->position = vector3d(0,0,0);
    employee_ent->think = employee_think;

    employee_ent->velocity.x = fmod((float)drand48(), 0.02) - 0.02;
    employee_ent->velocity.y = fmod((float)drand48(), 0.02) - 0.02;

    return employee_ent;
}