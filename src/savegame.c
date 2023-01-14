#include "savegame.h"
#include "random.h"
#include "globals.h"
#include "level.h"
#include "banking.h"
#include "vegetables.h"

#pragma bss-name (push, "SAVEGAME")
unsigned int saved_magic_number;
unsigned char saved_level_number;
int saved_rnd_seed;
unsigned char saved_player_max_health;
unsigned char saved_player_health;

unsigned char saved_seed_inventory[VEGGIE_TYPE_COUNT];
Veggie saved_player_party[PLAYER_PARTY_SIZE];
unsigned char saved_map_x, saved_map_y;
coordinate saved_player_x, saved_player_y;

#pragma bss-name (pop)

extern unsigned char romBankMirror;

void save_game_vars() {
    unsigned char i;
    unsigned char tmp = romBankMirror;
    ChangeRomBank(BANK_SAVEGAME);
    saved_magic_number = SAVEGAME_MAGIC_INT;
    saved_rnd_seed = rnd_seed;

    for(i = 0; i < PLAYER_PARTY_SIZE; ++i) {
        saved_player_party[i] = player_party[i];
    }

    for(i = 0; i < VEGGIE_TYPE_COUNT; ++i) {
        saved_seed_inventory[i] = seed_inventory[i];
    }

    saved_map_x = world_map_x;
    saved_map_y = world_map_y;
    saved_player_x = player_x;
    saved_player_y = player_y;

    saved_player_health = player_health;
    saved_player_max_health = player_max_health;
    ChangeRomBank(tmp);
}

void load_game_vars() {
    unsigned char i;
    unsigned char tmp = romBankMirror;
    ChangeRomBank(BANK_SAVEGAME);
    rnd_seed = saved_rnd_seed;
    for(i = 0; i < PLAYER_PARTY_SIZE; ++i) {
        player_party[i] = saved_player_party[i];
    }

    for(i = 0; i < VEGGIE_TYPE_COUNT; ++i) {
        seed_inventory[i] = saved_seed_inventory[i];
    }

    world_map_x = saved_map_x;
    world_map_y = saved_map_y;
    player_x = saved_player_x;
    player_y = saved_player_y;

    player_max_health = saved_player_max_health;
    ChangeRomBank(tmp);
}

char test_save_magic_number() {
    unsigned char tmp = romBankMirror;
    char cmpresult;
    ChangeRomBank(BANK_SAVEGAME);
    cmpresult = saved_magic_number == SAVEGAME_MAGIC_INT;
    ChangeRomBank(tmp);
    return cmpresult;
}

void clear_save() {
    unsigned char tmp = romBankMirror;
    ChangeRomBank(BANK_SAVEGAME);
    saved_magic_number = 0;
    ChangeRomBank(tmp);
}