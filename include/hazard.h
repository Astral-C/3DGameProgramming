#ifndef __HAZARD_H__
#define __HAZARD_H__
#include "entity.h"
#include "gfc_primitives.h"

typedef enum {
    PUDDLE_HAZARD,
    SPIKE_HAZARD,
    FIRE_HAZARD
} HazardType;

typedef struct {
    int damage;
    int timer;
    int move_dir;
    float offset;

    Vector3D direction;
    Model* animated; //animated spike
} Spike;

typedef struct {
    int damageTimer;
    Model* water_surface;
    Model* water_basin;
    Vector4D water_anim;
} Puddle;

Entity* spawn_spike(Vector3D pos);
Entity* spawn_puddle(Vector3D pos);
Entity* spawn_fire(Vector3D pos);

#endif