#include "enemies.h"
#include "gametank.h"
#include "drawing_funcs.h"
#include "globals.h"
#include "tilemap.h"
#include "music.h"
#include "random.h"
#include "banking.h"
#include "battle.h"
#include "vegetables.h"

void LD_enemy_index(char i);
void ST_enemy_index(char i);
char enemy_index_on_screen(char i);

char tempEnemyIndex;
unsigned char last_attacker_id;
MobState tempEnemy;

MobState enemies[MAX_ENEMIES];

char *mapMessage = "map filled";
char *maxHpMessage = "hearts increased";

unsigned char enemy_count = 0;

unsigned char enemy_type_slots[ENEMY_TYPE_NUM_SLOTS];
unsigned char enemy_type_used_slots = 0;

char ignoreCamera = 0;

extern const unsigned char* SineTable;

extern const unsigned char* EnemySprites_VEGGIES;

extern const Frame EnemyFrames_VEGGIES;

#define RANGE_IMPACT 96
#define RANGE_MELEE 144
#define RANGE_MELEE_HITSIZE 96
#define RANGE_SHOOT 256
#define RANGE_ARROW 48
#define RANGE_HURTBOX 112
#define RANGE_LARGE_HURTBOX 192
#define RANGE_ORC_CHARGE 384

//Added to coordinates when moving
char enemy_speeds[] = {
    4, //RAT
};

//Update if anim_frame & mask == 0
unsigned char move_frame_mask[] = {
    3, //RAT
};

unsigned char anim_state_offsets[] = {
    0, //INACTIVE
    0, //NORMAL
    24, //KNOCKBACK
    12 //ATTACKING
};

//Flags mask for mob types
unsigned char enemy_type_flags[] = {
    EFLAGS_PRESET_DEFAULTENEMY, //RAT
};

const Frame* enemyFrameData[] = {
    &EnemyFrames_VEGGIES,
};
const char** enemySpriteSheets[] = {
    &EnemySprites_VEGGIES
};

const char loadSpecial[] = {
    0,0,0,0,0,0,0,0,4,4,0
};

void attack_sound_for_enemy(char type) {
    switch (type)
    {
    case ENEMY_TYPE_VEGGIES:
            do_noise_effect(30, 64, 8);
            break;
    }
}

char enemy_banks[] = {
    BANK_MONSTERS
};

void load_enemy_type(char type) {
    char i, j;
    ChangeRomBank(enemy_banks[type]);
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        if(enemy_type_slots[i] == type) {
            return;
        }
    }
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        if(enemy_type_slots[i] == ENEMY_TYPE_NONE) {
            enemy_type_slots[i] = type;
            ++enemy_type_used_slots;
            load_spritesheet(enemySpriteSheets[type], i+2);
            return;
        }
    }
}

void clear_enemy_slots() {
    char i;
    enemy_type_used_slots = 0;
    for(i = 0; i < ENEMY_TYPE_NUM_SLOTS; ++i) {
        enemy_type_slots[i] = ENEMY_TYPE_NONE;
    }
}

unsigned char random_loaded_enemy_slot() {
    char selected;
    do {
        selected = (((unsigned char) rnd()) & 0x7F) % enemy_type_used_slots;
    } while ((enemy_type_flags[enemy_type_slots[selected]] & EFLAGS_PLACEABLE) == 0);
    return selected;
}

void init_enemy(char slot, MobState* enemy) {
    char type = enemy_type_slots[slot];
    enemy->mode = ENEMY_STATE_NORMAL; //change to set on per-enemy basis
    enemy->slot = slot;
    enemy->anim_flip = SPRITE_FLIP_NONE;
    enemy->on_screen = 0;
    switch(rnd()&3) {
        case 0:
            enemy->anim_flip = SPRITE_FLIP_X;
        case 1:
            enemy->anim_dir = ANIM_DIR_SIDE;
            break;
        case 2:
            enemy->anim_dir = ANIM_DIR_UP;
            break;
        case 3:
            enemy->anim_dir = ANIM_DIR_DOWN;
            break;
    }
    switch(type) {
    case ENEMY_TYPE_VEGGIES:
            enemy->health = 1;
            break;
    }
}

void clear_enemies() {
    char i;
    ignoreCamera = 0;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        enemies[i].mode = ENEMY_STATE_INACTIVE;
        enemies[i].x.i = 0;
        enemies[i].y.i = 0;
        enemies[i].anim_flip = 0;
        enemies[i].anim_dir = 0;
        enemies[i].anim_frame = 0; 
    }
    enemy_count = 0;
}

void close_wall_traps() {
    int r, c;
    for(r = 0; r < MAP_H; ++r) {
        for(c = 0; c < MAP_W; ++c) {
            if((tile_at_cell(c, r) & SPECIAL_TILE_MASK) == SPECIAL_TILE_WALLTRAP)
            {
                set_tile(c << 8, r << 8, WALL_TILE);
            }
        }
    }
}

void open_gates() {
    int r, c;
    for(r = 0; r < MAP_H; ++r) {
        for(c = 0; c < MAP_W; ++c) {
            if((tile_at_cell(c, r) & SPECIAL_TILE_MASK) == SPECIAL_TILE_GATE)
            {
                set_tile(c << 8, r << 8, GROUND_TILE);
            }
        }
    }
}

void place_enemies() {
    unsigned char i=0, r, c, attempt;
    /*for(r = 0; r < MAP_H; ++r) {
        for(c = 0; c < MAP_W; ++c) {
            attempt = tile_at_cell(c, r);
            if(attempt & SPECIAL_TILE_SPAWN_ENEMY) {
                attempt = attempt & (SPECIAL_TILE_SPAWN_ENEMY-1);
                init_enemy(attempt, &enemies[i]);
                enemies[i].x.i = ((c << TILE_ORD) | 16)<<3;
                enemies[i].y.i = ((r << TILE_ORD) | 16)<<3;
                enemy_index_on_screen(i);
                ++i;
                ++enemy_count;
            }
        }
    }*/
    if(i == 0) {
        do {
            i = 0;
                //save the last three slots for projectile pool
                for(; i < MAX_ENEMIES; ++i) {
                    enemies[i].mode = 0;
                    if(i < MAX_ENEMIES - RESERVED_PROJECTILE_SLOTS) {
                        enemies[i].x.i = (rnd() & 0x1F00) | 128;
                        enemies[i].y.i = (rnd() & 0x1F00) | 128;
                        init_enemy(0, &enemies[i]);
                        attempt = 0;
                     while((!character_tilemap_check(enemies[i].x, enemies[i].y)) ||
                            (enemies[i].x.i == player_x.i && enemies[i].y.i == player_y.i)) {
                            enemies[i].x.i = (rnd() &  0x1F00) | 128;
                            enemies[i].y.i = (rnd() &  0x1F00) | 128;
                            attempt++;
                            if(attempt == (i < 2 ? 16 : 5)) {
                                enemies[i].mode = 0;
                            }
                        }
                        if(enemies[i].mode != 0) {
                            enemy_count++;
                            enemies[i].vegtype = (rnd() & 255) % 11;
                            enemies[i].hostile = 1;
                        }
                    }
                    enemy_index_on_screen(i);
                }
        } while(enemy_count == 0);
    }
}

MobState* next_enemy_slot() {
    char i;
    for(i = 0; i < MAX_ENEMIES; ++i) {
        if(enemies[i].mode == 0) {
            return &(enemies[i]);
        }
    }
    return &(enemies[0]);
}

#pragma codeseg (push, "CODE2");
void draw_enemies() {
    unsigned char i = 0;
    MobState *enemy = enemies;
    queue_flags_param = DMA_GCARRY;
    for(; i < MAX_ENEMIES; ++i) {
        if(enemy->mode != ENEMY_STATE_INACTIVE) {
            if(enemy->on_screen) {
                    LD_enemy_index(i);
                    tempEnemyIndex = i;
                    if(tempEnemy.mode == ENEMY_STATE_ITEM) {
                        QueuePackedSprite(&HeroFrames,
                            (tempEnemy.x.i - camera_x.i) >> 4, (tempEnemy.y.i - camera_y.i) >> 4,
                            tempEnemy.anim_frame,
                            0,
                            GRAM_PAGE(1) | bankflip | BANK_CLIP_X | BANK_CLIP_Y, 0);
                    } else {
                        QueuePackedSprite(enemyFrameData[enemy_type_slots[tempEnemy.slot]],
                        (tempEnemy.x.i - camera_x.i) >> 4, (tempEnemy.y.i - camera_y.i) >> 4,
                        tempEnemy.vegtype,
                        tempEnemy.anim_flip,
                        GRAM_PAGE(tempEnemy.slot + 2) | bankflip | BANK_CLIP_X | BANK_CLIP_Y, 0);
                    }
                } 
        }
        ++enemy;
    }
}


void face_player() {
    temp3 = player_x.i - tempEnemy.x.i;
    temp4 = player_y.i - tempEnemy.y.i;
    temp1 = temp3;
    temp2 = temp4;
    
    if(temp3 < 0) {
        temp1 = -temp1;
    }
    if(temp4 < 0) {
        temp2 = -temp2;
    }
    if(temp1 > temp2) {
        if(temp3 > 0) {
            tempEnemy.anim_dir = ANIM_DIR_SIDE;
            tempEnemy.anim_flip = SPRITE_FLIP_NONE;
        } else {
            tempEnemy.anim_dir = ANIM_DIR_SIDE;
            tempEnemy.anim_flip = SPRITE_FLIP_X;
        }
    } else {
        if(temp4 > 0) {
            tempEnemy.anim_dir = ANIM_DIR_DOWN;
            tempEnemy.anim_flip = SPRITE_FLIP_NONE;
        } else {
            tempEnemy.anim_dir = ANIM_DIR_UP;
            tempEnemy.anim_flip = SPRITE_FLIP_NONE;
        }
    }
}

void check_player_attack(char hitsize) {
    if(player_hitbox_damage) {
        temp1 = player_x.i + (player_dir_x>>1) - tempEnemy.x.i;
        temp2 = player_y.i + (player_dir_y>>1) - tempEnemy.y.i;
        if(temp1 < 0) {
            temp1 = -temp1;
        }
        if(temp2 < 0) {
            temp2 = -temp2;
        }
        if(temp1 + temp2 < hitsize) {
            tempEnemy.mode = ENEMY_STATE_KNOCKBACK;
            tempEnemy.anim_frame = 0;
            if(tempEnemy.health)
                --tempEnemy.health;
            do_noise_effect(70, -12, 8);
            face_player();
        }
    }
}

char player_dist_check(char dist) {
    temp1 = player_x.i - tempEnemy.x.i;
    temp2 = player_y.i - tempEnemy.y.i;
    if(temp1 < 0) {
        temp1 = -temp1;
    }
    if(temp2 < 0) {
        temp2 = -temp2;
    }
    return (temp1 + temp2) < dist;
}

void check_impact_attack(char dist) {
    if(!((player_anim_state == PLAYER_STATE_HITSTUN) || (player_anim_state == PLAYER_STATE_DEAD))) {
        if(player_dist_check(dist)) {
            player_anim_state = PLAYER_STATE_HITSTUN;
            last_attacker_id = tempEnemyIndex;
            init_battle(tempEnemy.vegtype);
            player_anim_frame = 0;
            --player_health;
            player_face_enemy();
            do_noise_effect(80, -8, 10);
            tempEnemy.mode = ENEMY_STATE_KNOCKBACK;
        }
    }
}

char nextProjectile = MAX_ENEMIES-RESERVED_PROJECTILE_SLOTS;
MobState* spawn_projectile(char slot, int x, int y) {
    MobState* newProjectile = &enemies[nextProjectile];
    init_enemy(slot, newProjectile);
    enemies[nextProjectile].anim_dir = tempEnemy.anim_dir;
    enemies[nextProjectile].anim_flip = tempEnemy.anim_flip;
    enemies[nextProjectile].x.i = x;
    enemies[nextProjectile].y.i = y;
    ++nextProjectile;
    if(nextProjectile == MAX_ENEMIES) {
        nextProjectile = MAX_ENEMIES - RESERVED_PROJECTILE_SLOTS;
    }
    return newProjectile;
}

void circle_position(int* x, int* y, char angle) {
    temp3 = angle;
    temp4 = SineTable[temp3];
    temp3 = (temp3 + 64) & 255;
    temp3 = SineTable[temp3];
    *x = ((temp3 << 1) + (temp3));
    *y = ((temp4 << 1) + (temp4));
    *x += player_x.i - 384;
    *y += player_y.i - 384;
}

void big_circle(int* x, int* y, char angle) {
    temp3 = angle;
    temp4 = SineTable[temp3];
    temp3 = (temp3 + 192) & 255;
    temp3 = SineTable[temp3];
    *x = temp3 << 2;
    *y = temp4 << 2;
    *x += player_x.i - 512;
    *y += player_y.i - 512;
}

unsigned char update_enemies() {
    static char i = 0;
    static char speed, type, flags, cnt, movemask, moveflags, atkflags;
    static signed char chase_offset_x = 0;
    static signed char chase_offset_y = 0;
    MobState *proj;
    MobState *enemy = &enemies[i];
    cnt = 0;
    if(i == 0) {
        chase_offset_x = 0;
        chase_offset_y = 0;
    }
    for(; (i < MAX_ENEMIES) && (cnt < MAX_ENEMIES_PER_FRAME); ++i) {
        //Check enemy is on screen
        via[ORB] = 0x80;
        via[ORB] = 0x02;
        if(enemy->mode && (enemy_index_on_screen(i) || ignoreCamera)) {
            via[ORB] = 0x80;
            via[ORB] = 0x42;
            LD_enemy_index(i);
            tempEnemyIndex = i;
            type = enemy_type_slots[tempEnemy.slot];
            speed = enemy_speeds[type];
            
            if(player_health == 0) {
                speed = 0;
            }
            if(i == MAX_ENEMIES-RESERVED_PROJECTILE_SLOTS) {
                chase_offset_x = 0;
                chase_offset_y = 0;
            }
            flags = enemy_type_flags[type];
            moveflags = flags & EFLAGS_MOVETYPE_MASK;
            atkflags = flags & EFLAGS_ATTACK_MASK;
            if(!tempEnemy.hostile) {
                moveflags = EFLAGS_MOVETYPE_STATIC;
                atkflags = EFLAGS_ATTACK_NONE;
            }
            switch(tempEnemy.mode) {
            case ENEMY_STATE_NORMAL:
                temp1 = tempEnemy.x.i;
                temp2 = tempEnemy.y.i;
                ++(tempEnemy.anim_frame);
                movemask = move_frame_mask[type];
                if((tempEnemy.anim_frame & movemask) == 0) {
                    ++cnt;
                    switch(moveflags) {
                    case EFLAGS_MOVETYPE_CHASE:
                        if(atkflags != EFLAGS_ATTACK_IMPACT) {
                            player_x.i += chase_offset_x;
                            player_y.i += chase_offset_y;
                        }
                        if(tempEnemy.x.i > player_x.i) {
                            tempEnemy.x.i -= speed;
                        } else if(tempEnemy.x.i < player_x.i) {
                            tempEnemy.x.i += speed;
                        }
                        if((~flags & EFLAGS_PHASING) && !character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.x.i = temp1;
                        }
                        if(tempEnemy.y.i > player_y.i) {
                            tempEnemy.y.i -= speed;
                        } else if (tempEnemy.y.i < player_y.i) {
                            tempEnemy.y.i += speed;
                        }
                        if((~flags & EFLAGS_PHASING) && !character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.y.i = temp2;
                        }
                        
                        if(atkflags != EFLAGS_ATTACK_IMPACT) {
                            player_x.i -= chase_offset_x;
                            player_y.i -= chase_offset_y;
                        }
                        break;
                    case EFLAGS_MOVETYPE_DIRECTION:
                        switch(tempEnemy.anim_dir) {
                            case ANIM_DIR_DOWN:
                                tempEnemy.y.i += speed;
                                break;
                            case ANIM_DIR_UP:
                                tempEnemy.y.i -= speed;
                                break;
                            case ANIM_DIR_SIDE:
                                if(tempEnemy.anim_flip) {
                                    tempEnemy.x.i -= speed;
                                } else {
                                    tempEnemy.x.i += speed;
                                }
                                break;
                        }
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.mode = ENEMY_STATE_INACTIVE; 
                        }
                        break;
                    case EFLAGS_MOVETYPE_TELEPORT:
                        if((tempEnemy.anim_frame & 63) == 0) {
                            //do {
                                circle_position(&tempEnemy.x.i, &tempEnemy.y.i, rnd() & 255);
                            //} while(!character_tilemap_check(tempEnemy.x.i, tempEnemy.y.i));
                        }
                        face_player();
                        break;
                    } //end switch MOVETYPE
                    if(player_health > 0) {
                        switch(atkflags) {
                            case EFLAGS_ATTACK_MELEE:
                                temp1 = player_x.i - tempEnemy.x.i;
                                temp2 = player_y.i - tempEnemy.y.i;
                                if(temp1 < 0) {
                                    temp1 = -temp1;
                                }
                                if(temp2 < 0) {
                                    temp2 = -temp2;
                                }
                                if(temp1 + temp2 < RANGE_MELEE) {
                                    tempEnemy.mode = ENEMY_STATE_ATTACKING;
                                    tempEnemy.anim_frame = 0;
                                    attack_sound_for_enemy(type);
                                }
                            case EFLAGS_ATTACK_IMPACT:
                                check_impact_attack(RANGE_IMPACT);
                                break;
                            case EFLAGS_ATTACK_PROJECTILE:
                                temp1 = (tempEnemy.x.i == player_x.i+chase_offset_x) || (tempEnemy.y.i == player_y.i+chase_offset_y);
                                if(temp1) {
                                    tempEnemy.mode = ENEMY_STATE_ATTACKING;
                                    tempEnemy.anim_frame = 0;
                                    face_player();
                                }
                                break;
                        }
                    }
                }
                if(!tempEnemy.hostile) {
                    check_player_attack((flags & EFLAGS_LARGE) ? RANGE_LARGE_HURTBOX : RANGE_HURTBOX);
                }
                chase_offset_x = tempEnemy.y.i - player_y.i;
                chase_offset_y = player_x.i - tempEnemy.x.i;
                break;//out of ENEMY_STATE_NORMAL
            case ENEMY_STATE_KNOCKBACK:
                ++(tempEnemy.anim_frame);
                temp1 = tempEnemy.x.i;
                temp2 = tempEnemy.y.i;
                temp3 = 16;
                switch(tempEnemy.anim_dir) {
                            case ANIM_DIR_DOWN:
                                tempEnemy.y.i -= 8;
                                break;
                            case ANIM_DIR_UP:
                                tempEnemy.y.i += 8;
                                break;
                            case ANIM_DIR_SIDE:
                                if(tempEnemy.anim_flip) {
                                    tempEnemy.x.i += 8;
                                } else {
                                    tempEnemy.x.i -= 8;
                                }
                                break;
                        }
                        if(!character_tilemap_check(tempEnemy.x, tempEnemy.y)) {
                            tempEnemy.x.i = temp1;
                            tempEnemy.y.i = temp2;
                        }
                if(tempEnemy.anim_frame == temp3) {
                    if(tempEnemy.health) {
                        tempEnemy.mode = ENEMY_STATE_NORMAL;
                    } else {
                        add_to_party(tempEnemy.vegtype);
                        tempEnemy.mode = ENEMY_STATE_INACTIVE;
                        if(i < (MAX_ENEMIES - RESERVED_PROJECTILE_SLOTS)) {
                            --enemy_count;
                            if((i < 2) && ignoreCamera == 0) {
                                tempEnemy.mode = ENEMY_STATE_ITEM;
                                tempEnemy.anim_frame = i ? ITEM_TYPE_MAXHP : ITEM_TYPE_MAP;
                            } else {
                                if((rnd() & 31) < 3) {
                                    tempEnemy.mode = ENEMY_STATE_ITEM;
                                    tempEnemy.anim_frame = ITEM_TYPE_HEART;
                                }
                            }
                            if(enemy_count == 0) {
                                open_gates();
                                if(ignoreCamera) {
                                    stop_music();
                                }
                            }
                        }
                        
                    }
                }
                break;//out of ENEMY_STATE_KNOCKBACK
            case ENEMY_STATE_ATTACKING:
                check_impact_attack(RANGE_IMPACT);
                temp3 = 8;
                temp4 = 16;
                if(tempEnemy.anim_frame & 128) {
                    tempEnemy.anim_frame -= 128;
                    ++(tempEnemy.anim_frame);
                } else {
                    tempEnemy.anim_frame += 128;
                }
                if(tempEnemy.anim_frame == temp3) {
                    switch(atkflags) {
                    case EFLAGS_ATTACK_MELEE:
                        if(!((player_anim_state == PLAYER_STATE_HITSTUN) || (player_anim_state == PLAYER_STATE_DEAD))) {
                            //calc melee hitbox
                            temp3 = 0;
                            temp4 = 0;
                            switch(tempEnemy.anim_dir) {
                                case ANIM_DIR_DOWN:
                                    temp4 = RANGE_MELEE;
                                    break;
                                case ANIM_DIR_UP:
                                    temp4 = -RANGE_MELEE;
                                    break;
                                case ANIM_DIR_SIDE:
                                    if(tempEnemy.anim_flip) {
                                        temp3 = -RANGE_MELEE;
                                    } else {
                                        temp3 = RANGE_MELEE;
                                    }
                                    break;
                            }
                        
                            temp1 = tempEnemy.x.i + temp3 - player_x.i;
                            temp2 = tempEnemy.y.i + temp4 - player_y.i;
                            if(temp1 < 0) {
                                temp1 = -temp1;
                            }
                            if(temp2 < 0) {
                                temp2 = -temp2;
                            }
                            if(temp1 + temp2 < RANGE_MELEE_HITSIZE) {
                                player_anim_state = PLAYER_STATE_HITSTUN;
                                player_anim_frame = 0;
                                --player_health;
                                player_face_enemy();
                                do_noise_effect(80, -8, 10);
                            }
                        }
                        break;
                    case EFLAGS_ATTACK_PROJECTILE:
                        spawn_projectile(tempEnemy.slot+1, tempEnemy.x.i, tempEnemy.y.i);
                        break;
                    }
                }
                else if(tempEnemy.anim_frame == temp4) {
                    tempEnemy.anim_frame = 0;
                    tempEnemy.mode = ENEMY_STATE_NORMAL;                    
                }
                check_player_attack((flags & EFLAGS_LARGE) ? RANGE_LARGE_HURTBOX : RANGE_HURTBOX);
                break; //out of enemy state attack
            case ENEMY_STATE_ITEM:
                if(player_dist_check(RANGE_IMPACT)) {
                    switch(tempEnemy.anim_frame) {
                        case ITEM_TYPE_HEART:
                        if(player_health < player_max_health) {
                            ++player_health;
                        }
                        //play_track(MUSIC_TRACK_PICKUP, REPEAT_RESUME);
                        break;
                        case ITEM_TYPE_MAXHP:
                        ++player_max_health;
                        temp_msg_counter = 180;
                        message_string = maxHpMessage;
                        //play_track(MUSIC_TRACK_FANFARE, REPEAT_RESUME);
                        break;
                        case ITEM_TYPE_MAP:
                        do_fill_map = 1;
                        //play_track(MUSIC_TRACK_MAP_ITEM, REPEAT_RESUME);
                        temp_msg_counter = 180;
                        message_string = mapMessage;
                        break;
                    }
                    tempEnemy.mode = ENEMY_STATE_INACTIVE;
                }
                break; //out of enemy_state_item
            }
            ST_enemy_index(i);
        } else {
            via[ORB] = 0x80;
            via[ORB] = 0x42;
        }
        ++enemy;
    }
    if(i == MAX_ENEMIES) {
        i = 0;
    }
    return cnt;
}

void player_face_enemy() {
    temp3 = player_x.i - tempEnemy.x.i;
    temp4 = player_y.i - tempEnemy.y.i;
    temp1 = temp3;
    temp2 = temp4;
    
    if(temp3 < 0) {
        temp1 = -temp1;
    }
    if(temp4 < 0) {
        temp2 = -temp2;
    }
    if(temp1 > temp2) {
        if(temp3 < 0) {
            player_anim_dir = 4;
            player_anim_flip = 0;
            player_dir_x = 8;
            player_dir_y = 0;
        } else {
            player_anim_dir = 4;
            player_anim_flip = SPRITE_FLIP_X;
            player_dir_x = -8;
            player_dir_y = 0;
        }
    } else {
        if(temp4 < 0) {
            player_anim_flip = 0;
            player_anim_dir = 0;
            player_dir_x = 0;
            player_dir_y = 8;
        } else {
            player_anim_flip = 0;
            player_anim_dir = 8;
            player_dir_x = 0;
            player_dir_y = -8;
        }
    }
}
#pragma codeseg (pop);