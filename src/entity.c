#include <stdlib.h>
#include <string.h>
#include "gfc_vector.h"
#include "gf3d_camera.h"

#include "simple_logger.h"

#include "entity.h"

typedef struct
{
    Entity *entity_list;
    Entity **entity_refs; // used for draw calls
    Uint32  entity_count;
    
} EntityManager;

static EntityManager entity_manager = {0};
static Vector3D cameraPos = {0};

void entity_system_close()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        entity_free(&entity_manager.entity_list[i]);        
    }
    free(entity_manager.entity_list);
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity_system closed");
}

void entity_system_init(Uint32 maxEntities)
{
    int i;
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),maxEntities);
    entity_manager.entity_refs = gfc_allocate_array(sizeof(Entity*),maxEntities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list, cannot allocate ZERO entities");
        return;
    }
    entity_manager.entity_count = maxEntities;

    for(i = 0; i < maxEntities; i++){
        entity_manager.entity_refs[i] = &entity_manager.entity_list[i];
    }

    atexit(entity_system_close);
    slog("entity_system initialized");
}

Entity *entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.entity_list[i]._inuse = 1;
            gfc_matrix_identity(entity_manager.entity_list[i].modelMat);
            entity_manager.entity_list[i].scale.x = 1;
            entity_manager.entity_list[i].scale.y = 1;
            entity_manager.entity_list[i].scale.z = 1;
            
            entity_manager.entity_list[i].color = gfc_color(1,1,1,1);
            entity_manager.entity_list[i].selectedColor = gfc_color(1,1,1,1);
            
            return &entity_manager.entity_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}

void entity_free(Entity *self)
{
    if (!self)return;
    //MUST DESTROY
    gf3d_model_free(self->model);
    memset(self,0,sizeof(Entity));
}


void entity_draw(Entity *self)
{
    if (!self)return;
    if (self->hidden)return;
    gf3d_model_draw(self->model,self->modelMat,gfc_color_to_vector4f(self->color),vector4d(1,1,1,1));
    if(self->draw != NULL){
        self->draw(self);
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(&entity_manager.entity_list[i]);
    }
}

int dist_sort(const Entity** self, const Entity** other){
    return ((*self)->position.y - cameraPos.y) < ((*other)->position.y - cameraPos.y);
}

void entity_draw_all_sorted()
{
    cameraPos = gf3d_camera_get_position();
    qsort(entity_manager.entity_refs, entity_manager.entity_count, sizeof(Entity*), dist_sort);
    
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!(*entity_manager.entity_refs[i])._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(entity_manager.entity_refs[i]);
    }
}

void entity_think(Entity *self)
{
    if (!self)return;
    if (self->think)self->think(self);
}

void entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_think(&entity_manager.entity_list[i]);
    }
}


void entity_update(Entity *self)
{
    if (!self)return;
    // HANDLE ALL COMMON UPDATE STUFF
    
    vector3d_add(self->position,self->position,self->velocity);
    vector3d_add(self->velocity,self->acceleration,self->velocity);
    
    gfc_matrix_identity(self->modelMat);
    
    gfc_matrix_scale(self->modelMat,self->scale);
    gfc_matrix_rotate_by_vector(self->modelMat,self->modelMat,self->rotation);
    gfc_matrix_translate(self->modelMat,self->position);
    
    if (self->update)self->update(self);
}

void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_update(&entity_manager.entity_list[i]);
    }
}

/*eol@eof*/
