#include "enemy.h"
#include "employee.h"

void generic_enemy_think(Entity* self){
    self->velocity.x = Employees.focused->position.x - self->position.x;
    self->velocity.y = Employees.focused->position.y - self->position.y;
    vector3d_normalize(&self->velocity);

    if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, self->position.z, 3))){
        Employees.focused->health-=10;
    }

    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
    }
}

void spawn_rock(Vector3D position){
    Entity* rock = entity_new_at(position);
    rock->model = gf3d_model_load_full("models/cube.obj", "images/rock.png");
    rock->think = generic_enemy_think;
}

void rockman_think(Entity* self){
    if(self->team == 0){
        if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, self->position.z, 5))){
            spawn_rock(self->position);
            self->team = 15;
        }
    } else {
        self->team--;
    }
    
    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
    }
}

void spawn_enemy(EnemyType type, Vector3D pos){
    Entity* enemy = entity_new_at(pos);
    enemy->think = generic_enemy_think;
    switch (type)
    {
    case Rockman:
        enemy->think = rockman_think;
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/rockman.png");
        break;

    case Zombie:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/zombie.png");
        break;

    case Skeleton:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/skeleton.png");
        break;

    case Slime:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/slime.png");
        break;

    case Goblin:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/goblin.png");
        break;
    
    default:
        break;
    }
}