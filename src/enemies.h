#ifndef ENEMIES_H
#define ENEMIES_H

#include "coordinate.h"

#define MAX_ENEMIES 32
#define MAX_ENEMIES_PER_FRAME 3
#define ENEMY_TYPE_NUM_SLOTS 6
#define RESERVED_PROJECTILE_SLOTS 1

#define ENEMY_STATE_INACTIVE 0
#define ENEMY_STATE_NORMAL 1
#define ENEMY_STATE_KNOCKBACK 2
#define ENEMY_STATE_ATTACKING 3
#define ENEMY_STATE_ITEM 4

#define EFLAGS_PLACEABLE 1
#define EFLAGS_LARGE 2
#define EFLAGS_PHASING 4
#define EFLAGS_MOVETYPE_STATIC 0
#define EFLAGS_MOVETYPE_CHASE 8
#define EFLAGS_MOVETYPE_DIRECTION 16
#define EFLAGS_MOVETYPE_TELEPORT 24
#define EFLAGS_ATTACK_NONE 0
#define EFLAGS_ATTACK_IMPACT 32
#define EFLAGS_ATTACK_MELEE 64
#define EFLAGS_ATTACK_PROJECTILE 96
#define EFLAGS_ATTACK_CHARGE 128
//can add three more attack types

#define EFLAGS_MOVETYPE_MASK 24
#define EFLAGS_ATTACK_MASK 224

#define EFLAGS_PRESET_DEFAULTENEMY EFLAGS_PLACEABLE|EFLAGS_MOVETYPE_CHASE|EFLAGS_ATTACK_MELEE

#define ENEMY_TYPE_VEGGIES      0
#define ENEMY_TYPE_NONE     255

#define ITEM_TYPE_HEART 39
#define ITEM_TYPE_MAXHP 40
#define ITEM_TYPE_KEY   41
#define ITEM_TYPE_DOOR  42
#define ITEM_TYPE_MAP   43
#define ITEM_TYPE_JAR   44
#define ITEM_TYPE_CHEST 45
#define ITEM_TYPE_OPEN_CHEST 46

#define ANIM_DIR_DOWN 0
#define ANIM_DIR_SIDE 4
#define ANIM_DIR_UP 8

typedef struct MobState {
    unsigned char anim_frame, anim_dir, anim_flip, mode;
    coordinate x, y;
    unsigned char health, slot, on_screen, vegtype, hostile, d, e, f;
} MobState;

extern MobState enemies[MAX_ENEMIES];

extern unsigned char enemy_count;

extern unsigned char last_attacker_id;

void load_enemy_type(char type);

void clear_enemy_slots();

unsigned char random_loaded_enemy_slot();

void init_enemy(char type, MobState* enemy);

void clear_enemies();

void place_enemies();

MobState* next_enemy_slot();

void draw_enemies();

unsigned char update_enemies();

void player_face_enemy();

#endif