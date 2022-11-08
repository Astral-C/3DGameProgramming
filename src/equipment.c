#include "entity.h"
#include "employee.h"
#include "equipment.h"

EquipmentDef Equipment[6] = {
    { EQUIP_SLOT_MAX, EQUIP_EFFECT_MAX, EQUIP_STAT_MAX, "None", NULL },
    { EQUIP_HEAD, POISON, DEFENSE, "Toxic Crown", NULL },
    { EQUIP_LEGS, SLOW, SPEED, "You're Too Slow", NULL },
    { EQUIP_TORSO, POISON, ATTACK, "Toxic Blade", NULL },
    { EQUIP_HEAD, EXPLODE, EFFECT, "Mindblowing Hat", NULL },
    { EQUIP_HEAD, SLOW, DEFENSE, "Slowing Helmet", NULL }
};

void load_equipment_textures(){
    //later!
    Equipment[1].texture = gf3d_texture_load("images/toxic_crown.png");
    Equipment[2].texture = gf3d_texture_load("images/yts.png");
    Equipment[3].texture = gf3d_texture_load("images/toxic_blade.png");
    Equipment[4].texture = gf3d_texture_load("images/hat.png");
    Equipment[5].texture = gf3d_texture_load("images/slowing.png");
}


void equipment_think(Entity* self){
    if(vector3d_magnitude_between(self->position, Employees.focused->position) < 2){
        EmployeeData* employee = ((EmployeeData*)Employees.focused->customData);
        EquipmentDef* equipment_def = ((EquipmentDef*)self->customData);
        employee->equipment[equipment_def->slot] = equipment_def - Equipment;
        entity_free(self);
    }
}


Entity* spawn_random_equipment() {
    Entity* equip_ent = entity_new();

    equip_ent->model = gf3d_model_new();//gf3d_model_load_full("models/cube.obj", "images/hat.png");
    equip_ent->model->mesh = gf3d_mesh_load("models/cube.obj");
    equip_ent->scale.z = 0.5;
    equip_ent->position = vector3d(((rand() % 70) - 35) + 300, (rand() % 100) - 50, 0);
    equip_ent->think = equipment_think;

    equip_ent->customData = &Equipment[(rand() % 5) + 1];
    EquipmentDef* eq = (EquipmentDef*)equip_ent->customData;
    equip_ent->model->texture = eq->texture;

    return equip_ent;
}
