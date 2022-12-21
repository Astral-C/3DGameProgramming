#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "entity.h"

typedef enum {
    Rockman,
    Zombie,
    Skeleton,
    Slime,
    Goblin,
    ENEMY_TYPE_MAX
} EnemyType;

typedef struct {
    EnemyType type;
    Entity* entity;
    int attack_damage;
    Sphere hitbox;
} Enemy;


void spawn_enemy(EnemyType type, Vector3D pos);


#endif