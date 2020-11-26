#ifndef CHIP8_TYPE_H
#define CHIP8_TYPE_H

#include <stdint.h>

typedef struct ch8{
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

    void (*opcodeTable[16]) (struct ch8 *chip8);
} Chip8;

#endif
