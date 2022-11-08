#ifndef __EQUIPMENT_H__
#define __EQUIPMENT_H__

#include "gfc_types.h"
#include "gfc_text.h"
#include "gf3d_texture.h"
#include "entity.h"

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
    TextWord name;
    Texture* texture;
} EquipmentDef;

extern EquipmentDef Equipment[6];

Entity* spawn_random_equipment();
void load_equipment_textures();

// load equip defs from json?

#endif