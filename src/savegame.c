#include "savegame.h"
#include "random.h"
#include "globals.h"
#include "level.h"
#include "banking.h"

#pragma bss-name (push, "SAVEGAME")
unsigned int saved_magic_number;
unsigned char saved_level_number;
int saved_rnd_seed;
unsigned char saved_player_max_health;
unsigned char saved_player_health;

#pragma bss-name (pop)

extern unsigned char romBankMirror;

void save_game_vars() {
    unsigned char tmp = romBankMirror;
    ChangeRomBank(BANK_SAVEGAME);
    saved_magic_number = SAVEGAME_MAGIC_INT;
    saved_rnd_seed = rnd_seed;
    //saved_level_number = level_number;
    saved_player_health = player_health;
    saved_player_max_health = player_max_health;
    ChangeRomBank(tmp);
}

void load_game_vars() {
    unsigned char tmp = romBankMirror;
    ChangeRomBank(BANK_SAVEGAME);
    rnd_seed = saved_rnd_seed;
    //level_number = saved_level_number;
    //player_health = saved_player_health;
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