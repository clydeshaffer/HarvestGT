#include "enemies.h"
#include "tilemap.h"
#include "banking.h"
#include "music.h"
#include "globals.h"
#include "random.h"
#include "drawing_funcs.h"

unsigned char world_map_x = 128;
unsigned char world_map_y = 128;

extern char HomesteadMap;
extern char Forest0Map;
extern char DesertMap;
extern char MazeMap;
extern char BeachMap;
extern char ClearingMap;
extern char RoadMap;

char music_for_level() {
    return MUSIC_TRACK_MAIN;
}

void load_enemies_for_level(char levnum) {
    clear_enemy_slots();
    load_enemy_type((ENEMY_TYPE_VEGGIES));
}

void init_level(unsigned char x, unsigned char y, unsigned char traveling) {
        do_fill_map = 0;
        switch_tileset(0);
        //clear_spritebank(16);
        
        if(x == 128 && y == 128) {
            load_map(&HomesteadMap);
        } else if(x == 128 && y == 127) {
            load_map(&Forest0Map);
        } else if(x == 129 && y == 127) {
            load_map(&BeachMap);
        } else if(x == 130 && y == 127) {
            load_map(&ClearingMap);
        } else if(x == 127 && y == 127) {
            load_map(&MazeMap);
        } else if(x == 126 && y == 127) {
            load_map(&DesertMap);
        } else {
            load_map(&RoadMap);
        }

        //load_enemies_for_level(0);
        ChangeRomBank(BANK_COMMON);


        
            if(world_map_x != x) {
                player_x.b.msb = 31 - player_x.b.msb;
                player_x.b.lsb = 128;
            } else if(world_map_y != y) {
                player_y.b.msb = 31 - player_y.b.msb;
                player_y.b.lsb = 128;
            } else if(!traveling) {
                find_start_tile(&player_x, &player_y);
            }
        world_map_x = x;
        world_map_y = y;
        camera_x.i = player_x.i - 1024;
        camera_y.i = player_y.i - 1024;

        clear_enemies(); 
}