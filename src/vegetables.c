#include "vegetables.h"
#include "drawing_funcs.h"
#include "fontmap.h"
#include "random.h"
#include "ui/menu.h"

Veggie player_active_veggie;
Veggie encountered_veggie;
Veggie player_party[PLAYER_PARTY_SIZE];

/*
    unsigned char type, hp, maxhp, atk, def;
    unsigned char move0, move1, move2, move3;
    unsigned char speed, accuracy, dodge, crit;
    unsigned char mag, mag_def, mp, maxmp;
    unsigned char level, xp, element;
*/

char seed_inventory[12];
char selected_seed_type;

char seedsThrownX[32];
char seedsThrownY[32];
char seedsThrownType[32];
void resetSeeds() {
    char i;
    for(i = 0; i < 32; ++i) {
        seedsThrownType[i] = VEGGIE_TYPE_NONE;
    }
}

void plantedSeed(char x, char y, char type) {
    char i;
    for(i = 0; i < 32; ++i) {
        if(seedsThrownType[i] == VEGGIE_TYPE_NONE) {
            seedsThrownX[i] = x;
            seedsThrownY[i] = y;
            seedsThrownType[i] = type;
            return;
        }
    }
}

char findSeed(char x, char y) {
    char i;
    for(i = 0; i < 32; ++i) {
         if(seedsThrownType[i] != VEGGIE_TYPE_NONE) {
            if((seedsThrownX[i] == x)) {
                if(seedsThrownY[i] == y) {
                    return seedsThrownType[i];
                }
            }
        }
        
    }
    return VEGGIE_TYPE_NONE;
}

void add_to_party(char type) {
    char i;
    for(i = 0; i < 5; ++i) {
        if(player_party[i].type == VEGGIE_TYPE_NONE) {
            player_party[i] = veggie_templates[type];
            return;
        }
    }
}

void print_seed_inventory() {
    char leftAlign = cursorX;
    char i, didprint = 0;
    print("Seed Inventory");
    cursorY += 8;
    for(i = 1; i < 12; ++i) {
        if(seed_inventory[i] != 0) {
            didprint = 1;
            cursorX = leftAlign;
            print(veggie_names[i]);
            cursorX += 16;
            printBCDnum(seed_inventory[i]);
            cursorY += 8;
        }
    }
    if(!didprint) {
        cursorX = leftAlign;
        print("none");
    }
}

char randomInventorySeed() {
    char i, didprint = 0;
    for(i = 1; i < 12; ++i) {
        if(seed_inventory[i] != 0) {
            return i;
        }
    }
    return VEGGIE_TYPE_NONE;
}

void init_party_menu(char x, char y) {
    char i;
    set_menu_box(x, y, 48, 45);
    for(i = 0; i < PLAYER_PARTY_SIZE; ++i) {
        menu_texts[i] = veggie_names[player_party[i].type];
    }
    menu_text_count = PLAYER_PARTY_SIZE;
}

void init_seeds_menu(char x, char y) {
    char i;
    set_menu_box(x, y, 48, 110);
    menu_text_count = 0;
    for(i = 1; i < VEGGIE_TYPE_COUNT; ++i) {
        if(seed_inventory[i]) {
            menu_texts[menu_text_count] = veggie_names[i];
            ++menu_text_count;
        }
    }
}

char last_seed_thrown;
char seedArrowPosition;
void draw_seed_menu_arrow();

char *veggie_names[12] = {
    " ",
    "Corn",
    "Tomato",
    "Potato",
    "Carrot",
    "CaCarrot",
    "Ghost Pepper",
    "Cucumber",
    "Pumpkin",
    "Onion",
    "Broccoli",
    "Grapes"
};

char *attack_names[4] = {
    "Stalk",
    "Dig",
    "Wrath",
    "Powerup",

};

Veggie veggie_templates[11] = {
    {
        VEGGIE_TYPE_CORN, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_TOMATO, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_POTATO, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_CARROT, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_SUPER_CARROT, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_GHOST_PEPPER, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_CUCUMBER, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_PUMPKIN, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_ONION, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_BROCCOLI, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
    {
        VEGGIE_TYPE_GRAPES, 0x20, 0x20, 5, 5,
        1, 0, 0, 0,
        5, 5, 5, 1,
        0, 0, 2, 2,
        1, 0
    },
};