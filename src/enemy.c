#include "enemy.h"
#include "employee.h"
#include "equipment.h"
#include "world.h"
#include "gfc_input.h"

void generic_enemy_think(Entity* self){
    self->velocity.x = Employees.focused->position.x - self->position.x;
    self->velocity.y = Employees.focused->position.y - self->position.y;
    vector3d_normalize(&self->velocity);

    self->velocity.x *= 0.1 * (self->state == SLOW ? 0.5 : 1);
    self->velocity.y *= 0.1 * (self->state == SLOW ? 0.5 : 1);;

    Vector3D pos_step;
    vector3d_add(pos_step, self->position, self->velocity);
    int in_box = 0; //assume we are never in a bounding box
    for (size_t i = 0; i < dungeon.walkable_count; i++){
        in_box += gfc_point_in_box(pos_step, dungeon.walkable[i]);
    }
     if(in_box == 0){
        self->velocity.x = 0;
        self->velocity.y = 0; 
    }


    if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, 0, 2))){
       if(self->team == 0){
		Employees.focused->health -= 10;
		self->state = Equipment[Employees.employee_slots[Employees.focused_idx].equipment[EQUIP_HEAD]].effect_on_hit;
		if(self->state == EXPLODE){
			entity_free(self);
			return;
		}
	   }
    }


    if(self->team <= 0){
		if(self->state == POISON){
			self->health--;
		}
        self->team = 40;
    } else {
		self->team--;
	}

    if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, 0, 2.2)) && gfc_input_key_pressed("e")){
        self->health--;
    }


    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
    }

    if(self->health <= 0){
        entity_free(self);
    }
}

void rock_throw_think(Entity* self){
    self->velocity.x = Employees.focused->position.x - self->position.x;
    self->velocity.y = Employees.focused->position.y - self->position.y;
    vector3d_normalize(&self->velocity);

    self->velocity.x *= 0.05;
    self->velocity.y += 0.05;

    if(self->team-- <= 0){
        entity_free(self);
        return;
    }

    if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, 0, 1))){
        Employees.focused->health-=10;
		entity_free(self);
		return;
    }

    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
    }
}

void spawn_rock(Vector3D position){
    Entity* rock = entity_new_at(position);
    rock->model = gf3d_model_load_full("models/cube.obj", "images/rock.png");
    rock->think = rock_throw_think;
    rock->team = 400;
}

void rockman_think(Entity* self){
    if(self->team == 0){
        if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, 0, 6))){
            spawn_rock(self->position);
            self->team = 400;
        }
    } else {
        self->team--;
    }

    if(gfc_point_in_sphere(Employees.focused->position, gfc_sphere(self->position.x, self->position.y, 0, 2)) && gfc_input_key_pressed("e")){
        self->health--;
    }

    if(!((EmployeeData*)Employees.focused->customData)->in_dungeon){
        entity_free(self);
		return;
    }

    if(self->health <= 0){
        entity_free(self);
    }
}

void spawn_enemy(EnemyType type, Vector3D pos){
    Entity* enemy = entity_new_at(pos);
    enemy->think = generic_enemy_think;
	enemy->state = EQUIP_EFFECT_MAX;
    switch (type)
    {
    case Rockman:
        enemy->think = rockman_think;
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/rockman.png");
        enemy->health = 1;
        break;

    case Zombie:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/zombie.png");
        enemy->health = 5;
        break;

    case Skeleton:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/skeleton.png");
        enemy->health = 2;
        break;

    case Slime:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/slime.png");
        enemy->health = 1;
        break;

    case Goblin:
        enemy->model = gf3d_model_load_full("models/cube.obj", "images/goblin.png");
        enemy->health = 3;
        break;
    
    default:
        break;
    }
}