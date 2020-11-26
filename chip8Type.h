#ifndef CHIP8_TYPE_H
#define CHIP8_TYPE_H

#include <stdint.h>

typedef struct {
    uint8_t memory[4096];
    uint8_t video[2048];
    uint16_t stack[16];

    uint8_t V[16];
    uint8_t DT;
    uint8_t ST;
    uint8_t SP;
    uint16_t I;
    uint16_t PC;
} Chip8;

#endif
