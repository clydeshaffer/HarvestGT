#ifndef MENU_H
#define MENU_H

extern char *menu_texts[16];
extern char menu_text_count;

void set_menu_box(char x, char y, char w, char h);

void set_menu_msg(char* newMsg, char i);

void set_menu_index(char index);

char update_menu(int inputs, int last_inputs);

void render_menu();

void render_menu_text();

#endif