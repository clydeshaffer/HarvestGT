#include <zlib.h>
#include "globals.h"
#include "gametank.h"
#include "dynawave.h"
#include "drawing_funcs.h"
#include "tilemap.h"
#include "random.h"
#include "music.h"
#include "enemies.h"
#include "level.h"
#include "banking.h"
#include "savegame.h"
#include "fontmap.h"
#include "vegetables.h"
#include "screens/title.h"
#include "screens/battle.h"
#include "screens/pause.h"

int inputs = 0, last_inputs = 0;
int inputs2 = 0, last_inputs2 = 0;

extern void wait();
extern void nop5();

#pragma optimize(off)
void updateInputs(){
    char inputsA, inputsB;
    inputsA = *gamepad_2;
    inputsA = *gamepad_1;
    inputsB = *gamepad_1;

    last_inputs = inputs;
    inputs = ~((((int) inputsB) << 8) | inputsA);
    inputs &= INPUT_MASK_ALL_KEYS;

    inputsA = *gamepad_2;
    inputsB = *gamepad_2;
    last_inputs2 = inputs2;
    inputs2 = ~((((int) inputsB) << 8) | inputsA);
    inputs2 &= INPUT_MASK_ALL_KEYS;
}
#pragma optimize(on)

#define CAMERA_LIMIT 6128 // (TILE_SIZE * (MAP_W - VISIBLE_W + 1)) - 1

#define GAME_STATE_TITLE 0
#define GAME_STATE_PLAY 1
#define GAME_STATE_FADEIN 2
#define GAME_STATE_FADEOUT 3
#define GAME_STATE_PAUSED 4
#define GAME_STATE_ENDSCREEN 5
#define GAME_STATE_BATTLE 6

#define PLAYER_SPEED 16

unsigned char game_state = GAME_STATE_TITLE;
unsigned char game_over_timer = 256;

unsigned char stairs_known = 0;
unsigned char stairs_x = 0;
unsigned char stairs_y = 0;

#define PLAYER_START_X 48
#define PLAYER_START_Y 48

unsigned char go_to_next_level = 0;

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; ++i) {
        frameflag = 1;
        while(frameflag) {}
    }
}

void init_game_state(unsigned char new_state) {
    unsigned char i, old_state;
    old_state = game_state;
    game_state = new_state;
    stop_music();
    if(new_state == GAME_STATE_TITLE) {
        player_max_health = INITIAL_MAX_HEALTH;
        player_dir_x = 8;
        player_dir_y = 8;
        camera_x.i = 64;
        camera_y.i = 0;
        player_anim_frame = 0;
        player_health = 0;
        stairs_known = 0;
        updateInputs();
        init_level(128, 128, 0);
        ChangeRomBank(BANK_COMMON);
        init_title();
        player_party[0] = veggie_templates[0];
        player_party[1].type = VEGGIE_TYPE_NONE;
        player_party[2].type = VEGGIE_TYPE_NONE;
        player_party[3].type = VEGGIE_TYPE_NONE;
        player_party[4].type = VEGGIE_TYPE_NONE;
        selected_seed_type = 0;
        seedArrowPosition;
        for(i = 0; i < sizeof(seed_inventory); i++) {
            seed_inventory[i] = 0;
        }
        //play_track(MUSIC_TRACK_TITLE, REPEAT_NONE);
    } else if(new_state == GAME_STATE_PLAY) {
        player_dir_x = 0;
        player_dir_y = 128;
        player_anim_state = PLAYER_STATE_NEUTRAL;
        player_anim_frame = 0;
        stairs_known = 0;
        if(player_health != 0) {
            //next_level();
            ChangeRomBank(BANK_COMMON);
        }
        player_health = player_max_health;

        camera_x.i = player_x.i - 1024;
        camera_y.i = player_y.i - 1024;
        if(old_state == GAME_STATE_FADEOUT) {
            game_state = GAME_STATE_FADEIN;
        }
        play_track(MUSIC_TRACK_MAIN, REPEAT_LOOP);
    }
}

void draw_player() {
    char outFrame = 0;
    ChangeRomBank(BANK_COMMON);
    queue_flags_param = DMA_GCARRY;
    outFrame = (player_anim_frame >> 3) & 1;
    ++outFrame;
    if(player_anim_dir == ANIM_DIR_UP) {
        outFrame += 3;
    }
    if(player_anim_state == PLAYER_STATE_ATTACK) {
        outFrame = (player_anim_frame >> 3) % 3 + player_action_type;
    }
    QueuePackedSprite(&HeroFrames, (player_x.i - camera_x.i)>>4, (player_y.i - camera_y.i)>>4, outFrame, player_anim_flip, bankflip | GRAM_PAGE(1) | BANK_CLIP_X | BANK_CLIP_Y, 0);
}

void draw_hud() {
}

/*void fill_local_map() {
    static unsigned char i, j, k;
    static char *tmpptr_char, *tmpptr_char2;
    *vram_VX = 0;
    *vram_VY = 0;
    *vram_GX = 0;
    *vram_GY = 128;
    *vram_WIDTH = 1;
    *vram_HEIGHT = 1;
    *vram_START = 1;
    *vram_START = 0;
    flagsMirror = DMA_NMI | DMA_IRQ | frameflip;
    banksMirror = bankflip;
    *dma_flags = flagsMirror;
    *bank_reg = banksMirror;
    cursorX = camera_x.b.msb;
    cursorY = camera_y.b.msb;
    tmpptr_char = &vram[(cursorY << 7) + cursorX];
    tmpptr_char2 = &tiles[(cursorY << 5) + cursorX];
    for(i = 0; i < 5; ++i) {
        for(j = 0; j < 5; ++j) {
            k = *tmpptr_char2;
            if(k & GROUND_TILE) {
                if(k == STAIRS_TILE) {
                    *tmpptr_char = 28;
                    stairs_known = 1;
                    stairs_x = cursorX;
                    stairs_y = cursorY;
                } else {
                    *tmpptr_char = 75;
                }
            }
            cursorX++;
            ++tmpptr_char;
            ++tmpptr_char2;
        }
        cursorX -= 5;
        cursorY++;
        tmpptr_char2 += 27;
        tmpptr_char += 123;
    }
}*/

/*void fill_whole_map() {
    unsigned char i, j, k;
    char *tmpptr_char, *tmpptr_char2;
    *vram_VX = 0;
    *vram_VY = 0;
    *vram_GX = 0;
    *vram_GY = 128;
    *vram_WIDTH = 1;
    *vram_HEIGHT = 1;
    *vram_START = 1;
    *vram_START = 0;
    flagsMirror = DMA_NMI | DMA_IRQ | frameflip;
    banksMirror = bankflip;
    *dma_flags = flagsMirror;
    *bank_reg = banksMirror;
    cursorX = 0;
    cursorY = 0;
    tmpptr_char = vram;
    tmpptr_char2 = tiles;
    for(i = 0; i < MAP_H; ++i) {
        for(j = 0; j < MAP_W; ++j) {
            k = *tmpptr_char2;
            if(k & GROUND_TILE) {
                 if(k == STAIRS_TILE) {
                    *tmpptr_char = 28;
                    stairs_known = 1;
                    stairs_x = cursorX;
                    stairs_y = cursorY;
                } else {
                    *tmpptr_char = 75;
                }
            }
            cursorX++;
            ++tmpptr_char;
            ++tmpptr_char2;
        }
        cursorX = 0;
        cursorY++;
        tmpptr_char += 96;
    }
}*/

void main() {
    unsigned char i, j, k;
    unsigned int tx, ty;
    MobState* plantedEnemy;
    asm ("SEI");

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 16);
    wait();
    FillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-1, 16);
    wait();
    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | DMA_PAGE_OUT | DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    *bank_reg = BANK_SECOND_FRAMEBUFFER;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 16);
    wait();
    FillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-1, 16);
    wait();
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM;
    *bank_reg = 0;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM;
    *bank_reg = BANK_SECOND_FRAMEBUFFER;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;


    ChangeRomBank(BANK_INIT);
    init_dynawave();
    init_music();

    frameflip = 0;
    flagsMirror = DMA_NMI | DMA_IRQ | frameflip;
    bankflip = BANK_SECOND_FRAMEBUFFER;
    banksMirror = bankflip;

    load_spritesheet(&FontSprites, 8);
    load_spritesheet(&HeroSprites, 1);
    load_spritesheet(&BattleBG, 16);
    

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | frameflip;
    *dma_flags = flagsMirror;
    banksMirror = bankflip;
    *bank_reg = banksMirror;

    queue_start = 0;
    queue_end = 0;
    queue_pending = 0;
    vram[START] = 0;

    clear_enemy_slots();
    load_enemy_type(ENEMY_TYPE_VEGGIES);
    ChangeRomBank(BANK_COMMON);
    generate_map();

    via[DDRB] = 0xFF;

    asm ("CLI");

    for(game_over_timer = 0; game_over_timer < 116; game_over_timer+=4) {
        banksMirror = 0;
        *bank_reg = banksMirror;
        draw_fade(game_over_timer);
        Sleep(1);
    }

    init_game_state(GAME_STATE_TITLE);
    while(1){
        via[ORB] = 0x80;
        via[ORB] = 0x00;
        asm("SEI");
        queue_start = 0;
        queue_end = 0;
        queue_pending = 0;
        queue_count = 0;
        vram[START] = 0;
        asm("CLI");

        updateInputs();
        if(game_state == GAME_STATE_TITLE) {
            rnd();
            draw_world();
            draw_title_screen();
            ++player_anim_frame;
            if(player_anim_frame & 1) {
                camera_x.i += player_dir_x;
                if((camera_x.i + 32 > CAMERA_LIMIT) || (camera_x.i == 0)) {
                    player_dir_x = -player_dir_x;
                }
            }

            camera_y.i += player_dir_y;
            if((camera_y.i + 32 > CAMERA_LIMIT) || (camera_y.i == 0)) {
                player_dir_y = -player_dir_y;
            }
            if(update_title(inputs, last_inputs)) {
                init_game_state(GAME_STATE_PLAY);
            }
        }
        else if(game_state == GAME_STATE_PLAY) {    
            i = 0;
            tx = player_x.i;
            ty = player_y.i;
            player_hitbox_damage = 0;

            if(player_anim_state == PLAYER_STATE_HITSTUN) {
                game_state = GAME_STATE_BATTLE;
                play_track(MUSIC_TRACK_BATTLE, REPEAT_LOOP);
            } else if(player_anim_state == PLAYER_STATE_ATTACK) {
                i = (player_anim_frame >> 3) & 3;
                if(i == 1 || i == 2) {
                    player_hitbox_damage = 1;
                }
                i = 1;
                if(player_anim_frame == 16) {
                    switch(player_action_type) {
                        case PLAYER_ACTION_SEEDS:
                            if(tile_at(player_x.i + player_dir_x * 2, player_y.i + player_dir_y * 2) == 0x28) {
                                set_tile(player_x.i + player_dir_x * 2, player_y.i + player_dir_y * 2, 0x29);
                                plantedSeed((player_x.i + player_dir_x * 2) >> 8, (player_y.i + player_dir_y * 2) >> 8, last_seed_thrown);
                            }
                            break;
                        case PLAYER_ACTION_WATER:
                            if(tile_at(player_x.i + player_dir_x * 2, player_y.i + player_dir_y * 2) == 0x29) {
                                set_tile(player_x.i + player_dir_x * 2, player_y.i + player_dir_y * 2, 0x30);
                                j = findSeed((player_x.i + player_dir_x * 2) >> 8, (player_y.i + player_dir_y * 2) >> 8);
                                if(j != VEGGIE_TYPE_NONE) {
                                    plantedEnemy = next_enemy_slot();
                                    plantedEnemy->x.i = (player_x.i + player_dir_x * 2);
                                    plantedEnemy->y.i = (player_y.i + player_dir_y * 2);
                                    plantedEnemy->slot = 0;
                                    plantedEnemy->hostile = 0;
                                    plantedEnemy->vegtype = j - 1;
                                    plantedEnemy->mode = ENEMY_STATE_NORMAL;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
                if(player_anim_frame == 20) {
                    player_anim_state = PLAYER_STATE_NEUTRAL;
                }
            } else if(player_anim_state == PLAYER_STATE_NEUTRAL) {
                if(inputs & INPUT_MASK_START & ~last_inputs) {
                    init_pause_screen();
                    game_state = GAME_STATE_PAUSED;
                }
                else if(inputs & INPUT_MASK_A & ~last_inputs) {
                    player_anim_frame = 0;
                    player_anim_state = PLAYER_STATE_ATTACK;
                    player_action_type = PLAYER_ACTION_WATER;
                    do_noise_effect(95, 12, 4);
                    i = 1;
                } else if(inputs & INPUT_MASK_B & ~last_inputs) {
                    selected_seed_type = randomInventorySeed();
                    if(selected_seed_type != VEGGIE_TYPE_NONE) {
                        --seed_inventory[selected_seed_type];
                        last_seed_thrown = selected_seed_type;
                        if(seed_inventory[selected_seed_type] == 0) {
                            selected_seed_type = VEGGIE_TYPE_NONE;
                        }
                        player_anim_frame = 0;
                        player_anim_state = PLAYER_STATE_ATTACK;
                        player_action_type = PLAYER_ACTION_SEEDS;
                        do_noise_effect(95, 12, 4);
                        i = 1;
                    }
                } else if(inputs & INPUT_MASK_C & ~last_inputs) {
                    player_anim_frame = 0;
                    player_anim_state = PLAYER_STATE_ATTACK;
                    player_action_type = PLAYER_ACTION_HARVEST;
                    do_noise_effect(95, 12, 4);
                    i = 1;
                } else {
                    if(inputs & INPUT_MASK_RIGHT) {
                        player_anim_dir = 4;
                        player_anim_flip = 0;
                        i = 1;
                        player_x.i+=PLAYER_SPEED;
                        player_dir_x = 127;
                        player_dir_y = 0;
                    } else if(inputs & INPUT_MASK_LEFT) {
                        player_anim_dir = 4;
                        player_anim_flip = SPRITE_FLIP_X;
                        i = 1;
                        player_x.i-=PLAYER_SPEED;
                        player_dir_x = -127;
                        player_dir_y = 0;
                    }
                    if(!character_tilemap_check(player_x, player_y)) {
                        player_x.i = tx;
                    }

                    if(inputs & INPUT_MASK_DOWN) {
                        player_anim_dir = 0;
                        player_anim_flip = 0;
                        i = 1;
                        player_y.i+=PLAYER_SPEED;
                        player_dir_x = 0;
                        player_dir_y = 127;
                    } else if(inputs & INPUT_MASK_UP) {
                        player_anim_dir = 8;
                        player_anim_flip = 0;
                        i = 1;
                        player_y.i-=PLAYER_SPEED;
                        player_dir_x = 0;
                        player_dir_y = -127;
                    }
                    if(!character_tilemap_check(player_x, player_y)) {
                        player_y.i = ty;
                    }
                }
            }

            if(i == 1) {
                player_anim_frame+=2;
            }
            
            camera_x.i = player_x.i - 1024;
            camera_y.i = player_y.i - 1024;
            if(camera_x.i & 0x8000) {
                camera_x.i = 0;
            }
            if(camera_y.i & 0x8000) {
                camera_y.i = 0;
            }
            if(camera_x.i > CAMERA_LIMIT) {
                camera_x.i = CAMERA_LIMIT;
            }
            if(camera_y.i > CAMERA_LIMIT) {
                camera_y.i = CAMERA_LIMIT;
            }

            

            via[ORB] = 0x80;
            via[ORB] = 0x01;
            draw_world();
            update_enemies();
            via[ORB] = 0x80;
            via[ORB] = 0x41;

            draw_enemies();

            draw_player();
            
            draw_hud();
            if(player_anim_state == PLAYER_STATE_DEAD) {
                if(game_over_timer > 223) {
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, ((255 - game_over_timer) << 2), 64, 0, bankflip);
                    QueueSpriteRect();
                } else if (game_over_timer > 196) {
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, 112, 64, 0, bankflip);
                    QueueSpriteRect();
                    queue_flags_param = DMA_GCARRY;
                    SET_RECT(40, 60, 47, 8, 64, 96, 0, bankflip);
                    QueueSpriteRect();
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, ((game_over_timer & 0x3C) << 2), 64, 0, bankflip);
                    QueueSpriteRect();
                } else {
                    queue_flags_param = 0;
                    SET_RECT(0, 60, 127, 8, 112, 64, 0, bankflip);
                    QueueSpriteRect();
                    queue_flags_param = DMA_GCARRY;
                    SET_RECT(40, 60, 47, 8, 64, 96, 0, bankflip);
                    QueueSpriteRect();
                }
            }
        } else if((game_state == GAME_STATE_FADEOUT) || (game_state == GAME_STATE_FADEIN)) {
            draw_world();
            draw_enemies();
            draw_player();
            draw_hud();
            if(player_anim_state == PLAYER_STATE_DEAD) {
                queue_flags_param = 0;
                SET_RECT(0, 60, 127, 8, 112, 64, 0, bankflip);
                QueueSpriteRect();
                queue_flags_param = DMA_GCARRY;
                SET_RECT(40, 60, 47, 8, 64, 96, 0, bankflip);
                QueueSpriteRect();
            }
        } else if(game_state == GAME_STATE_PAUSED) {
            draw_world();
            draw_player();
            draw_pause_screen();
            /*SET_RECT(28, 16, 74,84, 156, 16, 0, bankflip)
            QueueSpriteRect();
            SET_RECT(48, 48, 32,32, 0, 128, 0, bankflip)
            QueueSpriteRect();*/
            if(update_pause_screen(inputs, last_inputs)) {
                game_state = GAME_STATE_PLAY;
            }
            /*queue_flags_param = DMA_GCARRY;
            if(stairs_known) {
                SET_RECT(46+(stairs_x), 46+(stairs_y), 5, 5, 107, 123, 0, bankflip);
                QueueSpriteRect();
            }
            SET_RECT(47+(player_x.b.msb), 47+(player_y.b.msb), 3, 3, 104, 120, 0, bankflip);
            QueueSpriteRect();*/
        } else if(game_state == GAME_STATE_BATTLE) {
            switch (update_battle(inputs, last_inputs)) {
                case BATTLE_SIGNAL_VICTORY:
                    enemies[last_attacker_id].mode = 0;
                case BATTLE_SIGNAL_FLED:
                    game_state = GAME_STATE_PLAY;
                    player_health = player_max_health;
                    player_anim_state = PLAYER_STATE_NEUTRAL;
                    play_track(MUSIC_TRACK_MAIN, REPEAT_LOOP);
                    break;
                case BATTLE_SIGNAL_DEFEAT:
                    init_game_state(GAME_STATE_TITLE);
                    break;
                default:
                    break;
            }
            draw_battle_screen();
        }
        
        CLB(16);

        while(queue_pending != 0) {
            asm("CLI");
            wait();
        }

        if(game_state == GAME_STATE_TITLE) {
            if(game_over_timer >= 4) {
                draw_fade(game_over_timer);
                game_over_timer -= 4;
            } else {
                draw_title_screen_postqueue();
            }
        } else if(game_state == GAME_STATE_FADEOUT) {
            if(game_over_timer < 116) {
                game_over_timer+=4;
            }
            draw_fade(game_over_timer);
            if(game_over_timer == 116) {
                if(player_anim_state == PLAYER_STATE_DEAD) {
                    init_game_state(GAME_STATE_TITLE);
                } else if(go_to_next_level) {
                    go_to_next_level = 0;
                    i = player_health;
                    init_game_state(GAME_STATE_PLAY);
                    player_health = i;
                } else {
                    game_state = GAME_STATE_FADEIN;
                }
            }
        } else if(game_state == GAME_STATE_FADEIN) {
            if(game_over_timer > 0) {
                game_over_timer -= 4;
            } else {
                game_state = GAME_STATE_PLAY;
            }
            draw_fade(game_over_timer);
        } else if(player_anim_state == PLAYER_STATE_DEAD) {
            game_over_timer--;
            if(game_over_timer == 0) {
                game_state = GAME_STATE_FADEOUT;
            }
        } else if(game_state == GAME_STATE_PLAY) {
            flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | frameflip;
            *dma_flags = flagsMirror;
            if(temp_msg_counter > 0) {
                cursorX = 0;
                cursorY = 108;
                print(message_string);
                --temp_msg_counter;
            } else {
                if(tile_at(player_x.i, player_y.i) == STAIRS_TILE) {
                    cursorX = 0;
                    cursorY = 108;
                    print("press b to enter");
                }
            }
            if(do_fill_map) {
                do_fill_map = 0;
                //fill_whole_map();
            } else {
                //fill_local_map();
            }
        } else if(game_state == GAME_STATE_PAUSED) {
            draw_pause_screen_postqueue();
        } else if(game_state == GAME_STATE_ENDSCREEN) {
            cursorX = 8;
            cursorY = 60;
            print("fiend is slain");
            cursorX = 64;
            cursorY = 90;
            print("for now");
            if(inputs & ~last_inputs & INPUT_MASK_START) {
                init_game_state(GAME_STATE_TITLE);
            }
        } else if(game_state == GAME_STATE_BATTLE) {
            draw_battle_screen_postqueue();
        }

        if(player_y.i < 64) {
            resetSeeds();
            init_level(world_map_x, world_map_y-1, 1);
            ChangeRomBank(BANK_COMMON);
        } else if(player_y.i > 8096) {
            resetSeeds();
            init_level(world_map_x, world_map_y+1, 1);
            ChangeRomBank(BANK_COMMON);
        } else if(player_x.i < 64) {
            resetSeeds();
            init_level(world_map_x-1, world_map_y, 1);
            ChangeRomBank(BANK_COMMON);
        } else if(player_x.i > 8096) {
            resetSeeds();
            init_level(world_map_x+1, world_map_y, 1);
            ChangeRomBank(BANK_COMMON);
        }

        Sleep(1);
        frameflip ^= DMA_PAGE_OUT;
        bankflip ^= BANK_SECOND_FRAMEBUFFER;
        flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | frameflip;
        banksMirror = bankflip;
        *dma_flags = flagsMirror;
        *bank_reg = banksMirror;
        ChangeRomBank(BANK_COMMON);
        tick_music();
        via[ORB] = 0x80;
        via[ORB] = 0x40;
    }
}
