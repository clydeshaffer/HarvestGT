#ifndef LEVEL_H
#define LEVEL_H

extern unsigned char world_map_x;
extern unsigned char world_map_y;

char music_for_level();

void init_level(unsigned char x, unsigned char y, unsigned char traveling);

#endif