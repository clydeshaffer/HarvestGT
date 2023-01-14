#include "ui/menu.h"
#include "drawing_funcs.h"
#include "title.h"
#include "fontmap.h"

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
    return update_menu(inputs, last_inputs);
}
