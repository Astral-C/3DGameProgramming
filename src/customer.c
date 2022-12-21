#include <time.h>
#include "customer.h"
#include "world.h"

CustomerManager Customers;

Texture* CustomerTextures[CUSTOMER_TYPES] = {0};

void load_customer_textures(){
    CustomerTextures[Ogre] = gf3d_texture_load("images/ogre.png");
    CustomerTextures[Druid] = gf3d_texture_load("images/druid.png");
    CustomerTextures[Wraith] = gf3d_texture_load("images/wraith.png");
    CustomerTextures[Demon] = gf3d_texture_load("images/demon.png");
    CustomerTextures[Golem] = gf3d_texture_load("images/golem.png");
    CustomerTextures[Gnome] = gf3d_texture_load("images/gnome.png");
    CustomerTextures[Fairy] = gf3d_texture_load("images/fairy.png");
    CustomerTextures[Kirin] = gf3d_texture_load("images/kirin.png");
    CustomerTextures[Phoenix] = gf3d_texture_load("images/phoenix.png");
}

void customer_manager_init(){
    for (size_t i = 0; i < CUSTOMER_MAX; i++){
        Customers.customer_slots[i].entity = spawn_customer();
        
        Customers.customer_slots[i].payout = rand() % 250;
        Customers.customer_slots[i].type = i % CUSTOMER_TYPES;
        Customers.customer_slots[i].want_type = rand() % EMPLOYEE_TYPES;
        Customers.customer_slots[i].leave_timer = (rand() % 200) + 50;
        
        Customers.customer_slots[i].entity->model = gf3d_model_new();
        Customers.customer_slots[i].entity->model->mesh = gf3d_mesh_load("models/cube.obj");
        Customers.customer_slots[i].entity->model->texture = CustomerTextures[Customers.customer_slots[i].type];
       
        Customers.customer_slots[i].entity->customData = &Customers.customer_slots[i];
    }
    
}

void customer_think(Entity* self){
    CustomerData* customer = ((CustomerData*)self->customData);

    if(Employees.employee_slots[Employees.focused_idx].in_dungeon && !self->hidden){
        self->hidden = 1;
    } else if(!Employees.employee_slots[Employees.focused_idx].in_dungeon && self->hidden){
        self->hidden = 0;
    }

    customer->leave_timer--;
    if(customer->leave_timer <= 0){
        entity_free(self);
        customer->entity = NULL;
        if(customer->targeter != NULL){ 
            ((EmployeeData*)customer->targeter->customData)->target = NULL;
        }
    }
}

void customer_manager_update(){
    for (size_t i = 0; i < CUSTOMER_MAX; i++){
        if(Customers.customer_slots[i].entity == NULL){
            Customers.customer_slots[i].entity = spawn_customer();

            Customers.customer_slots[i].entity->textureAnimationOffset.z = (rand() % 10 >= 5 ? 1.0f : 0.0f);

            Customers.customer_slots[i].payout = rand() % 250;
            Customers.customer_slots[i].type = rand() % CUSTOMER_TYPES;
            Customers.customer_slots[i].want_type = rand() % EMPLOYEE_TYPES;
            Customers.customer_slots[i].leave_timer = (rand() % 700) + 400 + (50 * shop.upgrades[COMFORT]);
            
            Customers.customer_slots[i].entity->model = gf3d_model_new();
            Customers.customer_slots[i].entity->model->mesh = gf3d_mesh_load("models/cube.obj");
            Customers.customer_slots[i].entity->model->texture = CustomerTextures[Customers.customer_slots[i].type];
        
            Customers.customer_slots[i].entity->customData = &Customers.customer_slots[i];
        }
    }
}

Entity* spawn_customer(){
    float percent_x = (float)(rand() % 100) / 100.0f;
    float percent_y = (float)(rand() % 100) / 100.0f;
    Entity* customer = entity_new_at(vector3d((shop.collision.x + 0.5f + ((shop.collision.w-0.5f) * percent_x)), (shop.collision.y + 0.5f + ((shop.collision.h - 0.5f) * percent_y)), 0));

    customer->think = customer_think;

    //employee_ent->velocity.x = fmod((float)drand48(), 0.08) - 0.04;
    //employee_ent->velocity.y = fmod((float)drand48(), 0.08) - 0.04;

    return customer;
}