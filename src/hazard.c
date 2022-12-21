#include "hazard.h"
#include "sg_util.h"
#include "employee.h"

#define SPIKE_MOVE_MAX 120
#define PUDDLE_DMG_TIMER 60

void spike_think(Entity* self){
    Spike* spike_data = (Spike*)self->customData;

    if(spike_data->timer >= SPIKE_MOVE_MAX){
        Sphere s;
        s.x = self->position.x + spike_data->direction.x;
        s.y = self->position.y + spike_data->direction.y;
        s.z = self->position.z + spike_data->direction.z;
        s.r = 5.0f;

        if(gfc_point_in_sphere(Employees.focused->position, s)){
            Employees.focused->health-=5;
        }

        spike_data->move_dir = -1;
    } else if(spike_data->timer == 0) {
        spike_data->move_dir = 1;
    }

    spike_data->timer += spike_data->move_dir;

    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
    }
}

void spike_draw(Entity* self){
    Matrix4 spikePos;
    Vector3D offset_vec;

    Spike* spike_data = (Spike*)self->customData;
    

    spike_data->offset = lerp(0.0f, 1.0f, (float)spike_data->timer / (float)SPIKE_MOVE_MAX);
    vector3d_copy(offset_vec, spike_data->direction);

    offset_vec.x *= spike_data->offset;
    offset_vec.y *= spike_data->offset;
    offset_vec.z *= spike_data->offset;

    gfc_matrix_copy(spikePos, self->modelMat);
    gfc_matrix_translate(spikePos, offset_vec);

    gf3d_model_draw(spike_data->animated,spikePos, vector4d(1,1,1,1), vector4d(1,1,1,1),vector4d(0,0,0,0));
    
}

void spike_free(Entity* self){
    Spike* spike_data = (Spike*)self->customData;
    if(self->customData != NULL){
        gf3d_model_free(spike_data->animated);
        free(self->customData);
    }
}

void puddle_draw(Entity* self){

    ((Puddle*)self->customData)->water_anim.x += 0.0005f;
    ((Puddle*)self->customData)->water_anim.y += 0.0005f;

    ((Puddle*)self->customData)->water_anim.z -= 0.0005f;
    ((Puddle*)self->customData)->water_anim.w += 0.0005f;

    gf3d_model_draw_water(((Puddle*)self->customData)->water_surface, self->modelMat, vector4d(1,1,1,1),vector4d(1,1,1,1),((Puddle*)self->customData)->water_anim);
    gf3d_model_draw_displacement(((Puddle*)self->customData)->water_basin, self->modelMat, vector4d(0.75,0.8,0.9,1),vector4d(1,1,1,1),((Puddle*)self->customData)->water_anim);

}

void puddle_think(Entity* self){
    if(((Puddle*)self->customData)->damageTimer-- <= 0 && gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, 0, 1))){
        Employees.focused->health -= 2;
        ((Puddle*)self->customData)->damageTimer = PUDDLE_DMG_TIMER;
    }
    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
    }
}

void puddle_die(Entity* self){
    gf3d_model_free(((Puddle*)self->customData)->water_surface);
    gf3d_model_free(((Puddle*)self->customData)->water_basin);
    gf3d_texture_free(((Puddle*)self->customData)->water_basin->displacementTexture);
    free(self->customData);
}


void fire_think(Entity* self){
    self->velocity.x = Employees.focused->position.x - self->position.x;
    self->velocity.y = Employees.focused->position.y - self->position.y;
    vector3d_normalize(&self->velocity);

    self->velocity.x *= 0.005;
    self->velocity.y *= 0.005;

    if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, self->position.z, 0.5))){
        Employees.focused->health-=100;
    }

    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
    }
}

Entity* spawn_puddle(Vector3D position){
    Entity* puddle = entity_new_at(position);
    puddle->model = gf3d_model_load_full("models/water_hazard_rim.obj", "images/dirt.png");
    puddle->drawPriority = 1;
    puddle->customData = malloc(sizeof(Puddle));
    memset(puddle->customData, 0, sizeof(Puddle));
    ((Puddle*)puddle->customData)->water_surface = gf3d_model_load_full("models/water_hazard_surface.obj", "images/water_overlay.png");
    ((Puddle*)puddle->customData)->water_basin  = gf3d_model_load_full("models/water_hazard_basin.obj", "images/dirt.png");
    ((Puddle*)puddle->customData)->water_basin->displacementTexture = gf3d_texture_load("images/astrSS04.png");
    ((Puddle*)puddle->customData)->damageTimer = PUDDLE_DMG_TIMER;

    puddle->position.z -= 0.5f;

    puddle->think = puddle_think;
    puddle->draw = puddle_draw;
    puddle->onDeath = puddle_die;

    return puddle;
}

Entity* spawn_spike(Vector3D position){
    Entity* spike = entity_new_at(position);
    spike->model = gf3d_model_load_full("models/SpikeBase.obj", "images/SpikesBase.png");

    spike->customData = malloc(sizeof(Spike));
    Spike* spike_data = (Spike*)spike->customData;

    spike_data->direction = vector3d(-1,0,0);
    spike_data->animated = gf3d_model_load_full("models/SpikeTop.obj", "images/SpikeTexture.png");
    spike_data->move_dir = 1;
    spike_data->timer = 0;

    spike->think = spike_think;
    spike->draw = spike_draw;
    spike->onDeath = spike_free;
    return spike;
}


Entity* spawn_fire(Vector3D position){
    Entity* fire = entity_new_at(position);
    fire->model = gf3d_model_load_full("models/cube.obj", "images/fireball.png");

    fire->think = fire_think;
    return fire;
}