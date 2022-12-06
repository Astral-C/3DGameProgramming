#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "entity.h"

typedef enum {
    Rockman,
    Zombie,
    Skeleton,
    Slime,
    Goblin
} EnemyType;

typedef struct {
    EnemyType type;
    Entity* entity;
    int attack_damage;
    Sphere hitbox;
} Enemy;

#endif