#include "ui/menu.h"
#include "savegame.h"
#include "drawing_funcs.h"
#include "pause.h"
#include "fontmap.h"
#include "gametank.h"
#include "vegetables.h"
#include "music.h"

#define PAUSE_STATE_MAIN 0
#define PAUSE_STATE_PARTY 1
#define PAUSE_STATE_SEEDS 2
#define PAUSE_STATE_MAP 3
#define PAUSE_STATE_SAVING 4

#pragma code-name (push, "CODE2");
#pragma rodata-name (push, "RODATA2");

char pause_screen_state;

void set_pause_mode(char mode) {
    switch (mode)
    {
    case PAUSE_STATE_MAIN:
        set_menu_box(72, 8, 48, 36);
        if(pause_screen_state > PAUSE_STATE_MAIN) {
            set_menu_index(pause_screen_state);
        } else {
            set_menu_index(1);
        }
        menu_texts[0] = "Status";
        menu_texts[1] = "Seeds";
        menu_texts[2] = "Map";
        menu_texts[3] = "Save";
        menu_text_count = 4;
        break;
    case PAUSE_STATE_PARTY:
        init_party_menu(72, 8);
        break;
    case PAUSE_STATE_SEEDS:
        init_seeds_menu(72, 8);
        break;
    case PAUSE_STATE_MAP:
        do_noise_effect(100, -100, 5);
        return;
    case PAUSE_STATE_SAVING:
        set_menu_box(16, 8, 108, 9);
        set_menu_msg("Saved the game!", 0);
        break;
    default:
        break;
    }
    pause_screen_state = mode;
}

void draw_pause_screen() {
    render_menu();
}

void init_pause_screen() {
    set_pause_mode(PAUSE_STATE_MAIN);
}

void draw_pause_screen_postqueue() {
    render_menu_text();
}

unsigned char update_pause_screen(int inputs, int last_inputs) {
    char selection = update_menu(inputs, last_inputs);
    switch(pause_screen_state) {
        case PAUSE_STATE_MAIN:
            if(selection) {
                set_pause_mode(selection);
                if(selection == PAUSE_STATE_SAVING) {
                    save_game_vars();
                }
            }
            break;
        case PAUSE_STATE_PARTY:
            break;
        case PAUSE_STATE_SEEDS:
            break;
        case PAUSE_STATE_MAP:
            break;
        case PAUSE_STATE_SAVING:
            if(INPUT_MASK_A & inputs & ~last_inputs) {
                set_pause_mode(PAUSE_STATE_MAIN);
            }
            break;
        default:
            break;
    }

    if(pause_screen_state != PAUSE_STATE_SAVING) {
        if(INPUT_MASK_START & inputs & ~last_inputs) {
            return PAUSE_SIGNAL_UNPAUSE;
        }
    }
    if(pause_screen_state != PAUSE_STATE_MAIN) {
        if(INPUT_MASK_B & inputs & ~last_inputs) {
            set_pause_mode(PAUSE_STATE_MAIN);
        }
    }
    return PAUSE_SIGNAL_NONE;
}

#pragma rodata-name (pop);
#pragma code-name (pop);