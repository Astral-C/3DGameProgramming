#ifndef __EMPLOYEE_H__
#define __EMPLOYEE_H__

#include "equipment.h"
#include "entity.h"
#include <stdint.h>

#define EMPLOYEE_MAX 10

typedef enum {
    Goblin,
    Knight,
    Ghost,
    Elf,
    CatPerson,
    EMPLOYEE_TYPES
} EmployeeType;

typedef struct {
    EmployeeType type;
    Entity* entity, *target;
    int move_timer;
    uint8_t health, attack, defense, speed, in_dungeon;
    uint32_t equipment[EQUIP_SLOT_MAX];
} EmployeeData;

typedef struct {
    int focused_idx, passive_effect_timer;
    Entity* focused;
    EmployeeData employee_slots[EMPLOYEE_MAX];
} EmployeeManager;


void generate_employee_data(EmployeeData* data);

Entity* spawn_employee();

void employee_manager_update();
void employee_manager_init();
void employee_manager_draw();

extern EmployeeManager Employees;

#endif