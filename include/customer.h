#ifndef __CUSTOMER_H__
#define __CUSTOMER_H__

#include "entity.h"
#include "employee.h"

#define CUSTOMER_MAX 25

typedef enum {
    Ogre,
    Druid,
    Wraith,
    Demon,
    Golem,
    Gnome,
    Fairy,
    Kirin,
    Phoenix,
    CUSTOMER_TYPES,
    CustomerTypeNone
} CustomerType;

extern Texture* CustomerTextures[CUSTOMER_TYPES];

typedef struct {
    CustomerType type;
    EmployeeType want_type;

    Entity* entity, *targeter;
    
    int leave_timer, payout;
} CustomerData;

typedef struct {
    CustomerData customer_slots[CUSTOMER_MAX];
} CustomerManager;


void load_customer_textures();

void customer_manager_update();
void customer_manager_init();

Entity* spawn_customer();

extern CustomerManager Customers;

#endif