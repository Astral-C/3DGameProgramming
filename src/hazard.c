#include "hazard.h"
#include "sg_util.h"
#include "employee.h"

#define SPIKE_MOVE_MAX 120

void spike_think(Entity* self){
    Spike* spike_data = (Spike*)self->customData;

    if(spike_data->timer >= SPIKE_MOVE_MAX){
        Sphere s;
        s.x = self->position.x + spike_data->direction.x;
        s.y = self->position.y + spike_data->direction.y;
        s.z = self->position.z + spike_data->direction.z;
        s.r = 2.0f;

        if(gfc_point_in_sphere(Employees.focused->position, s)){
            printf("spiked!\n");
        }

        spike_data->move_dir = -1;
    } else if(spike_data->timer == 0) {
        spike_data->move_dir = 1;
    }

    spike_data->timer += spike_data->move_dir;
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

    gf3d_model_draw(spike_data->animated,spikePos, vector4d(1,1,1,1), vector4d(1,1,1,1));
}

void spike_free(Entity* self){
    Spike* spike_data = (Spike*)self->customData;
    if(self->customData != NULL){
        gf3d_model_free(spike_data->animated);
        free(self->customData);
    }
}

Entity* spawn_spike(){
    Entity* spike = entity_new();
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
}

Entity* spawn_puddle(){

}

Entity* spawn_fire(){

}