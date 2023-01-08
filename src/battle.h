#ifndef BATTLE_H
#define BATTLE_H

#define BATTLE_SIGNAL_NONE 0
#define BATTLE_SIGNAL_VICTORY 1
#define BATTLE_SIGNAL_FLED 2
#define BATTLE_SIGNAL_DEFEAT 3

void draw_battle_screen();

void init_battle(unsigned char template_num);

void draw_battle_screen_postqueue();

unsigned char update_battle(int inputs, int last_inputs);

#endif