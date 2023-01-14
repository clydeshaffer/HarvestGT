#define SAVEGAME_MAGIC_INT 0xface

extern unsigned char saved_level_number;
extern int saved_rnd_seed;
extern unsigned char saved_player_max_health;
extern unsigned char saved_player_health;

void save_game_vars();

void load_game_vars();

void save_mobs();

void load_mobs();

char test_save_magic_number();

void clear_save();