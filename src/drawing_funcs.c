/*
 * GameTank-specific implementation of drawing functions
 */
#include <zlib.h>
#include "drawing_funcs.h"
#include "gametank.h"
#include "banking.h"

char cursorX, cursorY;

extern void wait();
extern void nop5();
extern void nop10();

void load_spritesheet(char* spriteData, char bank) {
    char oldFlags = flagsMirror;
    char oldBanks = banksMirror;
    char bankNum = bank & 7;
    char xbit = (bank & 8) << 4;
    char ybit = (bank & 16) << 3;
    flagsMirror = DMA_ENABLE;
    *dma_flags = flagsMirror;
    *vram_VX = 0;
    *vram_VY = 0;
    *vram_GX = xbit;
    *vram_GY = ybit;
    *vram_WIDTH = 1;
    *vram_HEIGHT = 1;
    *vram_START = 1;
    *vram_START = 0;

    flagsMirror = 0;
    *dma_flags = flagsMirror;
    banksMirror = bankflip | GRAM_PAGE(bank);
    *bank_reg = banksMirror;
    inflatemem(vram, spriteData);
    flagsMirror = oldFlags;
    banksMirror = oldBanks;
    *dma_flags = flagsMirror;
    *bank_reg = banksMirror;
}

void clear_spritebank(char bank) {
    char oldFlags = flagsMirror;
    char oldBanks = banksMirror;
    char bankNum = bank & 7;
    char xbit = (bank & 8) << 4;
    char ybit = (bank & 16) << 3;
    unsigned int i = 0;
    flagsMirror = DMA_ENABLE;
    *dma_flags = flagsMirror;
    *vram_VX = 0;
    *vram_VY = 0;
    *vram_GX = xbit;
    *vram_GY = ybit;
    *vram_WIDTH = 1;
    *vram_HEIGHT = 1;
    *vram_START = 1;
    *vram_START = 0;

    flagsMirror = 0;
    *dma_flags = flagsMirror;
    banksMirror = bankflip | GRAM_PAGE(bank);
    *bank_reg = banksMirror;
    for(i = 0; i < 16384; i++) {
        vram[i] = 0;
    }
    flagsMirror = oldFlags;
    banksMirror = oldBanks;
    *dma_flags = flagsMirror;
    *bank_reg = banksMirror;
}

#pragma bss-name (push,"ZEROPAGE")
#pragma data-name(push,"ZEROPAGE")
unsigned char queue_start = 0;
unsigned char queue_end = 0;
unsigned char queue_count = 0;
unsigned char queue_pending = 0;
#pragma bss-name (pop)
#pragma data-name(pop)

#define QUEUE_MAX 250
Frame temp_frame;

void pushRect();

#pragma codeseg (push, "CODE2");
void QueuePackedSprite(const Frame* sprite_table, char x, char y, char frame, char flip, char bank, char offset) {
    while(queue_count >= QUEUE_MAX) {
        asm("CLI");
        wait();
    }
    asm("SEI");
    temp_frame = sprite_table[frame];
    rect.b = bank;

    if(flip & SPRITE_FLIP_X) {
        rect.x = (x - temp_frame.w - temp_frame.x - 1);
    } else {
        rect.x = (temp_frame.x + x);
    }

    rect.y = (temp_frame.y + y);

    rect.gx = temp_frame.gx + (offset & 0x80);
    if(flip & SPRITE_FLIP_X) {
        rect.gx ^= 0xFF;
        rect.gx -= temp_frame.w - 1;
    }
    rect.gy = temp_frame.gy + (offset & 0x7F);
    rect.w = temp_frame.w | (flip & SPRITE_FLIP_X ? 128 : 0);
    rect.h = temp_frame.h;

    pushRect();

    if(queue_pending == 0) {
        NextQueue();
    }
    asm("CLI");
}


void PackedSprite(const Frame* sprite_table, char x, char y, char frame, char flip, char bank, char offset) {
    while(queue_count >= QUEUE_MAX) {
        asm("CLI");
        wait();
    }
    asm("SEI");
    temp_frame = sprite_table[frame];
    rect.b = bank;

    if(flip & SPRITE_FLIP_X) {
        rect.x = (x - temp_frame.w - temp_frame.x - 1);
    } else {
        rect.x = (temp_frame.x + x);
    }

    rect.y = (temp_frame.y + y);

    rect.gx = temp_frame.gx;
    if(flip & SPRITE_FLIP_X) {
        rect.gx ^= 0xFF;
        rect.gx -= temp_frame.w - 1;
    }
    rect.gy = temp_frame.gy + offset;
    rect.w = temp_frame.w | (flip & SPRITE_FLIP_X ? 128 : 0);
    rect.h = temp_frame.h;

    SpriteRectImmediate();

    if(queue_pending == 0) {
        NextQueue();
    }
    asm("CLI");
}
#pragma codeseg (pop);

void QueueSpriteRect() {
   if(rect.x > 127) {
        return;
    }
    if(rect.y > 127) {
        return;
    }
    if(rect.w == 0) {
        return;
    }
    if(rect.h == 0) {
        return;
    }
    if(queue_count >= QUEUE_MAX) {

        asm("CLI");
        wait();
    }

    if(rect.x + rect.w >= 128) {
        rect.w = 128 - rect.x;
    }
    if(rect.y + rect.h >= 128) {
        rect.h = 128 - rect.y;
    }

   asm("SEI"); 
    pushRect();


    if(queue_pending == 0) {
        NextQueue();
    }
    asm("CLI");
}

void SpriteRectImmediate() {
    if(rect.x > 127) {
        return;
    }
    if(rect.y > 127) {
        return;
    }
    if(rect.w == 0) {
        return;
    }
    if(rect.h == 0) {
        return;
    }

    if(rect.x + rect.w >= 128) {
        rect.w = 128 - rect.x;
    }
    if(rect.y + rect.h >= 128) {
        rect.h = 128 - rect.y;
    }
    if(queue_pending) {
        FlushQueue();
    }
    vram[VX] = rect.x;
    vram[VY] = rect.y;
    vram[GX] = rect.gx;
    vram[GY] = rect.gy;
    vram[WIDTH] = rect.w;
    vram[HEIGHT] = rect.h;
    vram[START] = 1;
}

void QueueFillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char c) {
    if(x > 127) {
        return;
    }
    if(y > 127) {
        return;
    }
    if(w == 0) {
        return;
    }
    if(h == 0) {
        return;
    }
    while(queue_count >= QUEUE_MAX) {
        asm("CLI");
        wait();
    }
    if(x + w >= 128) {
        w = 128 - x;
    }
    if(y + h >= 128) {
        h = 128 - y;
    }
    asm("SEI");

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    rect.gx = 0;
    rect.gy = 0;
    rect.b = bankflip;
    rect.c = ~c;
    queue_flags_param = DMA_COLORFILL_ENABLE | DMA_OPAQUE;
    pushRect();
    

    if(queue_pending == 0) {
        NextQueue();
    }
    asm("CLI");
}

void FlushQueue() {
    asm ("SEI");
    if(queue_pending != 0) {
        wait();
    }
    while(queue_end != queue_start) {
        NextQueue();
        wait();
    }
    vram[START] = 0;
    queue_pending = 0;
    asm ("CLI");
}

void CLB(char c) {
    QueueFillRect(0, 0, SCREEN_WIDTH-1, 7, c);
    QueueFillRect(0, 7, 1, SCREEN_HEIGHT-7, c);
    QueueFillRect(1, SCREEN_HEIGHT-8, SCREEN_WIDTH-1, 8, c);
    QueueFillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-8, c);
}

void CLS(char c) {
    *dma_flags = flagsMirror | DMA_COLORFILL_ENABLE | DMA_OPAQUE;
    queue_pending = 1;
    vram[VX] = 1;
    vram[VY] = 7;
    vram[GX] = 0;
    vram[GY] = 0;
    vram[WIDTH] = SCREEN_WIDTH-2;
    vram[HEIGHT] = SCREEN_HEIGHT-7-8;
    vram[COLOR] = ~c;
    vram[START] = 1;
    *dma_flags = flagsMirror;
}

void FillRect(char x, char y, char w, char h, char c) {
    *dma_flags = flagsMirror | DMA_COLORFILL_ENABLE | DMA_OPAQUE;
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = 0;
    vram[GY] = 0;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[COLOR] = ~c;
    vram[START] = 1;
    *dma_flags = flagsMirror;
}

void SpriteRect(char x, char y, char w, char h, char gx, char gy) {
    if(x + w >= 128) {
        w = 128 - x;
    }
    if(y + h >= 128) {
        h = 128 - y;
    }
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = gx;
    vram[GY] = gy;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[START] = 1;
}

void draw_fade(unsigned char opacity) {
    char oldFlags = flagsMirror;
    char oldBanks = banksMirror;
    flagsMirror |= DMA_ENABLE;
    flagsMirror &= ~DMA_GCARRY;
    flagsMirror = flagsMirror & ~DMA_OPAQUE;
    flagsMirror &= ~DMA_COLORFILL_ENABLE;
    *dma_flags = flagsMirror;
    banksMirror = banksMirror & 0xF8;
    *bank_reg = banksMirror;
    SpriteRect(0, 0, 127, 127, opacity&0xF0, 64);
    wait();
    flagsMirror = oldFlags;
    banksMirror = oldBanks;
    *dma_flags = flagsMirror;
    *bank_reg = banksMirror;
}

void printBCDnum(char num) {
    ChangeRomBank(BANK_COMMON);
    flagsMirror &= ~(DMA_COLORFILL_ENABLE | DMA_OPAQUE);
    flagsMirror |= DMA_GCARRY;
    *dma_flags = flagsMirror;
    vram[VX] = cursorX;
    vram[VY] = cursorY;
    rect = (&FontFrames)[(num & 15) + 54];
    vram[VX] = cursorX + rect.x;
    vram[VY] = cursorY + rect.y;
    vram[GX] = rect.gx + 128;
    vram[GY] = rect.gy;
    vram[WIDTH] = rect.w;
    vram[HEIGHT] = rect.h;
    vram[START] = 1;
    //wait();
    cursorX -= rect.w;
    vram[VX] = cursorX;
    vram[VY] = cursorY;
    rect = (&FontFrames)[(num >> 4) + 54];
    vram[VX] = cursorX + rect.x;
    vram[VY] = cursorY + rect.y;
    vram[GX] = rect.gx + 128;
    vram[GY] = rect.gy;
    vram[WIDTH] = rect.w;
    vram[HEIGHT] = rect.h;
    vram[START] = 1;
    //wait();
    cursorX -= rect.w;
}

void print(char* str) {
    ChangeRomBank(BANK_COMMON);
    flagsMirror &= ~(DMA_COLORFILL_ENABLE | DMA_OPAQUE);
    flagsMirror |= DMA_GCARRY;
    *dma_flags = flagsMirror;
    vram[WIDTH] = SPRITE_CHAR_W;
    vram[HEIGHT] = SPRITE_CHAR_H;
    while(*str != 0) {

        rect = (&FontFrames)[*str - 1];

        vram[GX] = rect.gx + 128;
        vram[GY] = rect.gy;
        vram[WIDTH] = rect.w;
        vram[HEIGHT] = rect.h;

        if(*str == 1) {
            cursorX = 0;
            cursorY += 8;
        } else {
            vram[VX] = cursorX + rect.x;
            vram[VY] = cursorY + rect.y;
            vram[START] = 1;
            //wait();
            cursorX += rect.w + 1;
        }
        str++;
        if(cursorX >= 128) {
            cursorX = 0;
            cursorY += 8;
        }
        if(cursorY >= 128) {
            cursorX = 0;
            cursorY = 0;
        }
    }
}