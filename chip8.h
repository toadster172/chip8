#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include "chip8Type.h"

void initializeSystem(Chip8 *chip8);
int loadRom(Chip8 *chip8, FILE *rom);
int readInstruction(Chip8 *chip8);

#endif
