#ifndef PAUSE_H
#define PAUSE_H

#define PAUSE_SIGNAL_NONE 0
#define PAUSE_SIGNAL_UNPAUSE 1

void draw_pause_screen();
void init_pause_screen();
void draw_pause_screen_postqueue();
unsigned char update_pause_screen(int inputs, int last_inputs);

#endif