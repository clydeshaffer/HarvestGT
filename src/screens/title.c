#include "ui/menu.h"
#include "music.h"
#include "savegame.h"
#include "drawing_funcs.h"
#include "title.h"
#include "fontmap.h"
#include "level.h"

#pragma code-name (push, "CODE2");
#pragma rodata-name (push, "RODATA2");

void draw_title_screen() {
    render_menu();
}

void init_title() {
    menu_texts[0] = "Continue";
    menu_texts[1] = "New Game";
    menu_text_count = 2;
    set_menu_box(30, 60, 48, 18);
    set_menu_index(1);
}

void draw_title_screen_postqueue() {
    //DRAW TITLE TEXT
    cursorX = 30;
    cursorY = 40;
    print("Veggie Tamer");
    render_menu_text();
}

unsigned char update_title(int inputs, int last_inputs) {
    switch(update_menu(inputs, last_inputs)) {
        case 1:
            if(test_save_magic_number()) {
                load_game_vars();
                init_level(world_map_x, world_map_y, 1);
                load_mobs();
                return TITLE_SIGNAL_LOAD;
            }
            do_noise_effect(100, -100, 5);
            return TITLE_SIGNAL_NONE;
        case 2:
            return TITLE_SIGNAL_NEW;
        default:
            return TITLE_SIGNAL_NONE;
    }
}

#pragma code-name (pop);
#pragma rodata-name (pop);