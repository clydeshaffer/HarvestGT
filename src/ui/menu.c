#include "drawing_funcs.h"
#include "gametank.h"
#include "menu.h"
#include "fontmap.h"

#define TEXT_BG_COLOR 212

#pragma code-name(push, "CODE2");

char *menu_texts[16];
char menu_text_count;
char textboxX, textboxY, textboxW, textboxH;
unsigned char menu_index;

char messageBuf[25];

void set_menu_msg(char* newMsg, char i) {
    while(*newMsg != 0) {
        messageBuf[i] = *newMsg;
        ++i;
        ++newMsg;
    }
    messageBuf[i] = 0;
    menu_texts[0] = messageBuf;
    menu_text_count = 1;
    menu_index = 255;
}

void set_menu_index(char index) {
    menu_index = index - 1;
}

void set_menu_box(char x, char y, char w, char h) {
    textboxX = x;
    textboxY = y;
    textboxW = w;
    textboxH = h;
}

char update_menu(int inputs, int last_inputs) {
    if(menu_index != 255) {
        if(INPUT_MASK_DOWN & inputs & ~last_inputs) {
            ++menu_index;
        }
        if(INPUT_MASK_UP & inputs & ~last_inputs) {
            --menu_index;
        }
        if(menu_index == 255) menu_index = 0;
        if(menu_index >= menu_text_count) menu_index = menu_text_count-1;
    }
    if(INPUT_MASK_A & inputs & ~last_inputs) {
        return menu_index+1;
    }
    return 0;
}

void render_menu() {
    QueueFillRect(textboxX, textboxY, textboxW, textboxH, TEXT_BG_COLOR);
    
    if(menu_index != 0xFF) {
        queue_flags_param = DMA_GCARRY;
        QueuePackedSprite(&FontFrames, textboxX - 4, textboxY + (menu_index << 3) + 4, '>' - 1, 0, bankflip, 128);
    }
}

void render_menu_text() {
    char i, left;
    cursorX = textboxX + 5;
    cursorY = textboxY + 5;
    left = cursorX;
    for(i = 0; i < menu_text_count; ++i) {
        print(menu_texts[i]);
        cursorX = left;
        cursorY += 8;
    }
}

#pragma code-name(pop);