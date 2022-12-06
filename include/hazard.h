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
    int damage;
} Puddle;

Entity* spawn_spike();
Entity* spawn_puddle();
Entity* spawn_fire();

#endif