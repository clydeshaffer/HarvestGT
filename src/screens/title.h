#ifndef TITLE_H
#define TITLE_H

#define TITLE_SIGNAL_NONE 0
#define TITLE_SIGNAL_NEW 2
#define TITLE_SIGNAL_LOAD 1

void draw_title_screen();

void init_title();

void draw_title_screen_postqueue();

unsigned char update_title(int inputs, int last_inputs);

#endif