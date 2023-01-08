#ifndef BANKING_H
#define BANKING_H

void ChangeRomBank(unsigned char banknum);

#define BANK_SAVEGAME   0x00

#define BANK_COMMON     0x80
#define BANK_TILES      0x81
#define BANK_MONSTERS   0x82
#define BANK_INIT       0x83
#define BANK_MONSTERS2  0x84
#define BANK_MONSTERS3  0x85

#endif