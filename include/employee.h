#ifndef __EMPLOYEE_H__
#define __EMPLOYEE_H__

#include "equipment.h"
#include <stdint.h>

typedef enum {
    Goblin,
    Knight,
    Ghost,
    Elf,
    CatPerson
} EmployeeType;

typedef struct {
    uint8_t health, attack, defense, speed;
    uint32_t equipment[EQUIP_SLOT_MAX];
} EmployeeData;

void GenerateEmployeeData(EmployeeData* data);

Entity* spawn_employee();


#endif