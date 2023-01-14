
#include "gametank.h"
#include "battle.h"
#include "drawing_funcs.h"
#include "vegetables.h"
#include "fontmap.h"
#include "ui/menu.h"
#include "music.h"

extern unsigned char EnemyFrames_VEGGIES;

char menuMode;

#define MENU_TYPE_NONE 0
#define MENU_TYPE_MESSAGE 1
#define MENU_TYPE_ACTION 2
#define MENU_TYPE_PARTY 3
#define MENU_TYPE_SKILL 4

char stateChangeTimer = 0;
char playerVeggieX = 0;
char wildVeggieX = 0;
char currentPartyIndex = 0;

#define BATTLE_STATE_MESSAGE 0
#define BATTLE_STATE_DECIDE 1
#define BATTLE_STATE_PLAYER_MOVE 2
#define BATTLE_STATE_ENEMY_MOVE 3
#define BATTLE_STATE_COMPLETE 4

char battle_state;
char next_battle_state;

#pragma codeseg (push, "CODE2");
#pragma rodata-name (push, "RODATA2");

char checkLoseCondition() {
    char i;
    for(i = 0; i < 5; i++) {
        if(player_party[i].type != VEGGIE_TYPE_NONE)
            return 0;
    }
    return 1;
}

void set_battle_menu_mode(char menuType) {
    char i;
    if(menuMode != menuType) {
        set_menu_index(1);
        if(menuMode == MENU_TYPE_PARTY)
            set_menu_index(3);
        else if(menuMode == MENU_TYPE_SKILL)
            set_menu_index(2);
    }
    menuMode = menuType;
    switch (menuType)
    {
    case MENU_TYPE_ACTION:
        set_menu_box(16, 8, 48, 36);
        menu_texts[0] = "Attack";
        menu_texts[1] = "Skill";
        menu_texts[2] = "Party";
        menu_texts[3] = "Flee";
        menu_text_count = 4;
        break;
    case MENU_TYPE_PARTY:
        set_menu_box(16, 8, 48, 45);
        for(i = 0; i < 5; ++i) {
            menu_texts[i] = veggie_names[player_party[i].type];
        }
        menu_text_count = 5;
        break;
    case MENU_TYPE_SKILL:
        set_menu_box(16, 8, 48, 18);
        menu_texts[0] = "???";
        menu_texts[1] = "???";
        menu_text_count = 2;
        break;
    case MENU_TYPE_MESSAGE:
        set_menu_box(16, 8, 108, 9);
        set_menu_index(0);
        break;
    default:
        break;
    }
}

void draw_battle_screen() {
    queue_flags_param = DMA_GCARRY;
    //Draw BG
    SET_RECT(0, 0, 127, 127, 0, 128, 0,  bankflip);
    QueueSpriteRect();
    //Draw player veggie
    QueuePackedSprite(&EnemyFrames_VEGGIES, 32 + playerVeggieX, 80, player_active_veggie.type - 1, 0, bankflip | 2, 0);

    //Draw wild veggie
    QueuePackedSprite(&EnemyFrames_VEGGIES, 96 + wildVeggieX, 80, encountered_veggie.type - 1, SPRITE_FLIP_X, bankflip | 2, 0);

    if(menuMode != MENU_TYPE_NONE) {
        render_menu();
    }
}

void draw_battle_screen_postqueue() {
    cursorX = 20;
    cursorY = 96;
    print(veggie_names[player_active_veggie.type]);
    cursorY = 104;
    printBCDnum(player_active_veggie.maxhp);
    cursorX -= 4;
    print("/");
    cursorX -= 12;
    printBCDnum(player_active_veggie.hp);
    
    
    cursorX = 70;
    cursorY = 96;
    print(veggie_names[encountered_veggie.type]);
    cursorY = 104;
    printBCDnum(encountered_veggie.maxhp);
    cursorX -= 4;
    print("/");
    cursorX -= 12;
    printBCDnum(encountered_veggie.hp);

    if(menuMode != MENU_TYPE_NONE)
        render_menu_text();
}



char get_first_party_veggie() {
    char i;
    for(i = 0; i < 5; ++i) {
        if(player_party[i].type != VEGGIE_TYPE_NONE) {
            return i;
        }
    }
    return 0;
}

void init_battle(unsigned char template_num) {
    set_menu_index(1);
    currentPartyIndex = get_first_party_veggie();
    player_active_veggie = player_party[currentPartyIndex];
    encountered_veggie = veggie_templates[template_num];
    battle_state = BATTLE_STATE_MESSAGE;
    next_battle_state = BATTLE_STATE_DECIDE;
    stateChangeTimer = 180;
    set_battle_menu_mode(MENU_TYPE_MESSAGE);
    set_menu_msg("Found ", 0);
    set_menu_msg(veggie_names[encountered_veggie.type], 6);
    do_noise_effect(64, 60, 16);
}

unsigned char update_battle(int inputs, int last_inputs) {
    unsigned char outsignal = BATTLE_SIGNAL_NONE;
    unsigned char menu_selection;
    if(battle_state == BATTLE_STATE_DECIDE) {
            playerVeggieX = 0;
            wildVeggieX = 0;
            menu_selection = update_menu(inputs, last_inputs);
            if(menu_selection) {
                --menu_selection;
                if(menuMode == MENU_TYPE_ACTION) {
                    switch (menu_selection)
                    {
                    case 0: //attack
                        asm("SED");
                        encountered_veggie.hp = encountered_veggie.hp - player_active_veggie.atk;
                        asm("CLD");
                        do_noise_effect(100, -100, 5);
                        battle_state = BATTLE_STATE_PLAYER_MOVE;
                        set_battle_menu_mode(MENU_TYPE_NONE);
                        stateChangeTimer = 30;
                        break;
                    case 1: //skill
                        set_battle_menu_mode(MENU_TYPE_SKILL);
                        break;
                    case 2: //party
                        set_battle_menu_mode(MENU_TYPE_PARTY);
                        break;
                    case 3: //flee
                        player_party[currentPartyIndex] = player_active_veggie;
                        battle_state = BATTLE_STATE_MESSAGE;
                        next_battle_state = BATTLE_STATE_COMPLETE;
                        stateChangeTimer = 180;
                        set_battle_menu_mode(MENU_TYPE_MESSAGE);
                        set_menu_msg("Ran away...", 0);
                        break;
                    default:
                        break;
                    }
                } else if(menuMode == MENU_TYPE_PARTY) {
                    if(player_party[menu_selection].type != VEGGIE_TYPE_NONE) {
                        if(menu_selection == currentPartyIndex) {
                            battle_state = BATTLE_STATE_MESSAGE;
                            next_battle_state = BATTLE_STATE_DECIDE;
                            stateChangeTimer = 180;
                            set_battle_menu_mode(MENU_TYPE_MESSAGE);
                            set_menu_msg("Already out!", 0);
                        } else {
                            if(player_active_veggie.hp != 0) {
                                player_party[currentPartyIndex] = player_active_veggie;
                            }
                            currentPartyIndex = menu_selection;
                            player_active_veggie = player_party[currentPartyIndex];
                            battle_state = BATTLE_STATE_MESSAGE;
                            next_battle_state = BATTLE_STATE_DECIDE;
                            stateChangeTimer = 180;
                            set_battle_menu_mode(MENU_TYPE_MESSAGE);
                            set_menu_msg("Sent out ", 0);
                            set_menu_msg(veggie_names[player_active_veggie.type], 9);
                        }
                    }
                }
            }

            if(INPUT_MASK_B & inputs & ~last_inputs) {
                if(menuMode == MENU_TYPE_PARTY) {
                    if(player_active_veggie.hp != 0) {
                        set_battle_menu_mode(MENU_TYPE_ACTION);
                    }
                } else {
                    if(menuMode != MENU_TYPE_ACTION) {
                        set_battle_menu_mode(MENU_TYPE_ACTION);
                    }
                }
            }
    } else if(battle_state == BATTLE_STATE_MESSAGE) {
        stateChangeTimer--;
        if(stateChangeTimer == 0 || (INPUT_MASK_A & inputs & ~last_inputs)) {
            battle_state = next_battle_state;
            if(battle_state == BATTLE_STATE_DECIDE) {
                set_battle_menu_mode(MENU_TYPE_ACTION);
            }
        }
    } else if(battle_state == BATTLE_STATE_PLAYER_MOVE) {
        wildVeggieX = 0;
        playerVeggieX = stateChangeTimer >> 3;
        stateChangeTimer--;
        if(stateChangeTimer == 0) {
            if(encountered_veggie.hp == 0) {
                player_party[currentPartyIndex] = player_active_veggie;
                battle_state = BATTLE_STATE_MESSAGE;
                next_battle_state = BATTLE_STATE_COMPLETE;
                stateChangeTimer = 180;
                set_battle_menu_mode(MENU_TYPE_MESSAGE);
                set_menu_msg("You win! Picked up seeds", 0);
                //asm("SED");
                seed_inventory[encountered_veggie.type] = seed_inventory[encountered_veggie.type]+1;
                //asm("CLD");
            } else {
                asm("SED");
                player_active_veggie.hp = player_active_veggie.hp - 1;//encountered_veggie.atk;
                asm("CLD");
                stateChangeTimer = 30;
                battle_state = BATTLE_STATE_ENEMY_MOVE;
                if(battle_state == BATTLE_STATE_DECIDE) {
                    set_battle_menu_mode(MENU_TYPE_ACTION);
                }
                do_noise_effect(88, -100, 5);
            }
        }

    } else if (battle_state == BATTLE_STATE_ENEMY_MOVE) {
        playerVeggieX = 0;
        wildVeggieX = 255 - (stateChangeTimer >> 3);
        stateChangeTimer--;
        if(stateChangeTimer == 0) {
            if(player_active_veggie.hp == 0) {
                player_party[currentPartyIndex].type = VEGGIE_TYPE_NONE;
            }
            if(checkLoseCondition()) {
                battle_state = BATTLE_STATE_MESSAGE;
                next_battle_state = BATTLE_STATE_COMPLETE;
                stateChangeTimer = 180;
                set_battle_menu_mode(MENU_TYPE_MESSAGE);
                set_menu_msg("Wiped out...", 0);
            } else {
                battle_state = BATTLE_STATE_DECIDE;
                if(battle_state == BATTLE_STATE_DECIDE) {
                    if(player_active_veggie.hp == 0) {
                    set_battle_menu_mode(MENU_TYPE_PARTY);
                    } else {
                        set_battle_menu_mode(MENU_TYPE_ACTION);
                    }
                }
            }
        }
    } else if(battle_state == BATTLE_STATE_COMPLETE) {
        if(encountered_veggie.hp == 0) {
            outsignal = BATTLE_SIGNAL_VICTORY;
        } else if(checkLoseCondition()) {
            outsignal = BATTLE_SIGNAL_DEFEAT;
        } else {
            outsignal = BATTLE_SIGNAL_FLED;
        }
    }
    return outsignal;
}
#pragma rodata-name (pop);
#pragma codeseg (pop);