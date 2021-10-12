#ifndef CHIP8_TYPE_H
#define CHIP8_TYPE_H

#include <stdint.h>

typedef struct _chip8 {
    uint8_t memory[4096];
    uint8_t video[2048];
    uint16_t stack[16];

    uint8_t V[16];
    uint8_t DT;
    uint8_t ST;
    uint8_t SP;
    uint16_t I;
    uint16_t PC;

    uint8_t opcodeNibble[4]; //Stores each hex character in opcode
    uint8_t illegalAccess; //Set to one in case of illegal instruction, quits if set

    uint8_t awaitKey;
    uint8_t mostRecentKey;
    uint16_t quirkFlags;

    void (*opcodeTable[16]) (struct _chip8 *chip8);
    
    void (*opcode0Table[256]) (struct _chip8 *chip8);
    void (*opcode8Table[16]) (struct _chip8 *chip8);
    void (*opcodeETable[256]) (struct _chip8 *chip8);
    void (*opcodeFTable[256]) (struct _chip8 *chip8);
} Chip8;

#endif
