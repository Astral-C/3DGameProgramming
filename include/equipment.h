#ifndef __EQUIPMENT_H__
#define __EQUIPMENT_H__

#include "gfc_types.h"

typedef enum {
    EQUIP_HEAD,
    EQUIP_TORSO,
    EQUIP_LEGS,
    EQUIP_SLOT_MAX
} EquipSlot;

typedef enum {
    POISON,
    SLOW,
    EXPLODE,
    EQUIP_EFFECT_MAX
} EquipEffect;

typedef enum {
    ATTACK,
    DEFENSE,
    SPEED,
    EFFECT,
    EQUIP_STAT_MAX
} EquipStats;

typedef struct {
    EquipSlot slot;
    EquipEffect effect_on_hit;
    EquipStats boost_stat;
    char name[32];
} EquipmentDef;

// load equip defs from json?

#endif