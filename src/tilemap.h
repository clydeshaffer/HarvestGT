/*
 * Functions for generating and collisionchecking the tilemap
 */

#ifndef TILEMAP_H
#define TILEMAP_H

#include "coordinate.h"

#define MAP_SIZE 1024
#define MAP_W 32
#define MAP_H 32
#define MAP_ORD 5

#define LS_MAP_ORD ls5
#define LS_TILE_ORD ls4

#define TILE_SIZE 16
#define TILE_ORD 4
#define VISIBLE_W 9
#define VISIBLE_H 9

#define HITBOX_X -48
#define HITBOX_Y 32
#define HITBOX_W 80
#define HITBOX_H 24

#define EMPTY_TILE 0
#define GROUND_TILE 32
#define WALL_TILE 64
#define STAIRS_TILE 96

#define SPECIAL_TILE_MASK 31
#define SPECIAL_TILE_MUNDANE 0
#define SPECIAL_TILE_START 1
#define SPECIAL_TILE_WALLTRAP 2
#define SPECIAL_TILE_GATE 3
#define SPECIAL_TILE_SPAWN_ENEMY 16

extern coordinate camera_x;
extern coordinate camera_y;
extern unsigned char tiles[MAP_SIZE];

void switch_tileset(unsigned char level_num);
void generate_map();
void load_map(char* data);
char tile_at(unsigned int pos_x, unsigned int pos_y);
char tile_at_cell(char x, char y);
void set_tile(unsigned int pos_x, unsigned int pos_y, char tile);
char character_tilemap_check(coordinate pos_x, coordinate pos_y);
void draw_world();
char find_start_tile(unsigned int* x, unsigned int* y);
void fill_map_screen();

#endif