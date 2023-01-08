#ifndef VEGETABLES_H
#define VEGETABLES_H

typedef struct Veggie {
    unsigned char type, hp, maxhp, atk, def;
    unsigned char move0, move1, move2, move3;
    unsigned char speed, accuracy, dodge, crit;
    unsigned char mag, mag_def, mp, maxmp;
    unsigned char level, xp, element;
} Veggie;

extern Veggie player_active_veggie;
extern Veggie encountered_veggie;
extern Veggie player_party[5];
extern Veggie veggie_templates[11];


extern char* veggie_names[12];
extern char* attack_names[4];
extern char selected_seed_type;
extern char seed_inventory[12];
extern char seedArrowPosition;
extern char last_seed_thrown;
void draw_seed_menu_arrow();

void print_seed_inventory();

void resetSeeds();

void plantedSeed(char x, char y, char type);

char randomInventorySeed();

void add_to_party(char type);

char findSeed(char x, char y);

#define ELEMENT_NONE 0
#define ELEMENT_SWEET 1
#define ELEMENT_SALTY 2
#define ELEMENT_SOUR 3
#define ELEMENT_BITTER 4
#define ELEMENT_UMAMI 5

#define VEGGIE_TYPE_NONE 0
#define VEGGIE_TYPE_CORN 1
#define VEGGIE_TYPE_TOMATO 2
#define VEGGIE_TYPE_POTATO 3
#define VEGGIE_TYPE_CARROT 4
#define VEGGIE_TYPE_SUPER_CARROT 5
#define VEGGIE_TYPE_GHOST_PEPPER 6
#define VEGGIE_TYPE_CUCUMBER 7
#define VEGGIE_TYPE_PUMPKIN 8
#define VEGGIE_TYPE_ONION 9
#define VEGGIE_TYPE_BROCCOLI 10
#define VEGGIE_TYPE_GRAPES 11

#endif