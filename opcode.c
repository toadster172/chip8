/* Copyright (C) 2020, 2021 Alice Shelton <AliceAShelton@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "opcode.h"
#include "chip8Type.h"
#include "sdlFunctions.h"

void clearScreen(Chip8 *chip8) { //Opcode 1, 0x00E0, CLS
    for(uint8_t i = 0; i < 32; i++) {
        for(uint8_t j = 0; j < 64; j++) {
            chip8->video[i * 64 + j] = 0x00;
        }
    }
    chip8->PC += 2;
}

void returnSubroutine(Chip8 *chip8) { //Opcode 2, 0x00EE, RET
    chip8->SP--;
    chip8->PC = chip8->stack[chip8->SP];
    chip8->PC += 2;
}

void jumpAdress(Chip8 *chip8) { //Opcode 3, 0x1nnn, JP
    chip8->PC = (chip8->opcodeNibble[1] << 8) + chip8->memory[chip8->PC + 1];
}

void callSubroutine(Chip8 *chip8) { //Opcode 4, 0x2nnn, CALL
    chip8->stack[chip8->SP] = chip8->PC;
    chip8->SP++;
    chip8->PC = (chip8->opcodeNibble[1] << 8) + chip8->memory[chip8->PC + 1];
}

void skipVXEqualsNum(Chip8 *chip8) { //Opcode 5, 0x3xkk, SE VX
    if(chip8->V[chip8->opcodeNibble[1]] == chip8->memory[chip8->PC + 1]) {
        chip8->PC += 2;
    }
    chip8->PC += 2;
}

void skipVXNotNum(Chip8 *chip8) { //Opcode 6, 0x4xkk, SNE VX
    if(chip8->V[chip8->opcodeNibble[1]] != chip8->memory[chip8->PC + 1]) {
        chip8->PC += 2;
    }
    chip8->PC += 2;
}

void skipVXEqualsVY(Chip8 *chip8) { //Opcode 7, 0x5xy0, SE VX, VY
    if(chip8->V[chip8->opcodeNibble[1]] == chip8->V[chip8->opcodeNibble[2]]) {
        chip8->PC += 2;
    }
    chip8->PC += 2;
}

void loadVXByte(Chip8 *chip8) { //Opcode 8, 0x6xkk, LD VX
    chip8->V[chip8->opcodeNibble[1]] = chip8->memory[chip8->PC + 1];
    chip8->PC += 2;
}

void addVXByte(Chip8 *chip8) { //Opcode 9, 0x7xnn, ADD VX
    chip8->V[chip8->opcodeNibble[1]] += chip8->memory[chip8->PC + 1];
    chip8->PC += 2;
}

void loadVXVY(Chip8 *chip8) { //Opcode 10, 0x8xy0, LD VX, VY
    chip8->V[chip8->opcodeNibble[1]] = chip8->V[chip8->opcodeNibble[2]];
    chip8->PC += 2;
}

void bitOrVXVY(Chip8 *chip8) { //Opcode 11, 0x8xy1, OR VX, XY
    chip8->V[chip8->opcodeNibble[1]] |= chip8->V[chip8->opcodeNibble[2]];
    chip8->PC += 2;
}

void bitAndVXVY(Chip8 *chip8) { //Opcode 12, 0x8xy2, AND VX, VY
    chip8->V[chip8->opcodeNibble[1]] &= chip8->V[chip8->opcodeNibble[2]];
    chip8->PC += 2;
}

void bitXorVXVY(Chip8 *chip8) { //Opcode 13, 0x8xy3, XOR VX, VY
    chip8->V[chip8->opcodeNibble[1]] ^= chip8->V[chip8->opcodeNibble[2]];
    chip8->PC += 2;
}
void addVXVY(Chip8 *chip8) { //Opcode 14, 0x8xy4, ADD VX, VY
    if(chip8->V[chip8->opcodeNibble[1]] + chip8->V[chip8->opcodeNibble[2]] > 0xFF) {
            chip8->V[0x0F] = 0x01;
    } else {
            chip8->V[0x0F] = 0x00;
    }
    chip8->V[chip8->opcodeNibble[1]] += chip8->V[chip8->opcodeNibble[2]];
    chip8->PC += 2;
}

void subtractVYVX(Chip8 *chip8) { //Opcode 15, 0x8xy5, SUB VX, VY
    if(chip8->V[chip8->opcodeNibble[1]] > chip8->V[chip8->opcodeNibble[2]]) {
            chip8->V[0x0F] = 0x01;
    } else {
            chip8->V[0x0F] = 0x00;
    }
    chip8->V[chip8->opcodeNibble[1]] -= chip8->V[chip8->opcodeNibble[2]];
    chip8->PC += 2;
}

void rightShiftVXVY(Chip8 *chip8) { //Opcode 16, 8xy6, SHR VX, VY
    chip8->V[0x0F] = chip8->V[chip8->opcodeNibble[2]] & 0x01;
    chip8->V[chip8->opcodeNibble[1]] = (chip8->V[chip8->opcodeNibble[2]] >> 1);
    chip8->PC += 2;
}

//Shift quirks ignore VY register, simly shifting VX left or right one
void rightShiftVX(Chip8 *chip8) { //Opcode 16Q, 0x8xy6, SHR VX
    chip8->V[0x0F] = chip8->V[chip8->opcodeNibble[1]] & 0x01;
    chip8->V[chip8->opcodeNibble[1]] >>= 0x01;
    chip8->PC += 2;
}

void subtractVXVY(Chip8 *chip8) { //Opcode 17, 0x8xy7, SUBN VX, VY
    if(chip8->V[chip8->opcodeNibble[2]] > chip8->V[chip8->opcodeNibble[1]]) {
            chip8->V[0x0F] = 0x01;
    } else {
            chip8->V[0x0F] = 0x00;
    }
    chip8->V[chip8->opcodeNibble[1]] = chip8->V[chip8->opcodeNibble[2]] - chip8->V[chip8->opcodeNibble[1]];
    chip8->PC += 2;
}

void leftShiftVXVY(Chip8 *chip8) { //Opcode 18, 0x8xyE, SHL VX, VY
    chip8->V[0x0F] = chip8->V[chip8->opcodeNibble[2]] >> 0x07;
    chip8->V[chip8->opcodeNibble[1]] = (chip8->V[chip8->opcodeNibble[2]] << 1);
    chip8->PC +=2;
}

//Shift quirks ignore VY register, simply shifting VX left or right one
void leftShiftVX(Chip8 *chip8) { //Opcode 18Q, 0x8xyE, SHL VX
    chip8->V[0x0F] = chip8->V[chip8->opcodeNibble[1]] >> 0x07;
    chip8->V[chip8->opcodeNibble[1]] <<= 0x01;
    chip8->PC += 2;
}

void skipVXNotEqualVY(Chip8 *chip8) { //Opcode 19, 0x9xy0, SNE VX, VY
    if(chip8->V[chip8->opcodeNibble[1]] != chip8->V[chip8->opcodeNibble[2]]) {
           chip8->PC += 2;
       }
       chip8->PC += 2;
}

void loadIAddress(Chip8 *chip8) { //Opcode 20, 0xAnnn, LD I
    chip8->I = (chip8->opcodeNibble[1] << 8) + chip8->memory[chip8->PC + 1];
    chip8->PC += 2;
}

void jumpPlusV0(Chip8 *chip8) { //Opcode 21, 0xBnnn, JP V0
    chip8->PC = (chip8->opcodeNibble[1] << 8) + chip8->memory[chip8->PC + 1] + chip8->V[0x00];
}

void generateRandom(Chip8 *chip8) { //Opcode 22, 0xCxkk, RND VX
    chip8->V[chip8->opcodeNibble[1]] = (rand() % 255) & chip8->memory[chip8->PC + 1];
    chip8->PC += 2;
}

void draw(Chip8 *chip8) { //Opcode 23, 0xDxyn, DRW VX, VY
    uint8_t pixelValue, erasedPixels = 0;
    uint16_t pos;
    for(uint8_t i = 0; i < chip8->opcodeNibble[3]; i++) {
        pos = (chip8->V[chip8->opcodeNibble[2]] + i) * 64 + chip8->V[chip8->opcodeNibble[1]];
        for(uint8_t j = 0; j < 8; j++) { //Writes one byte at a time to VRAM
            if(chip8->memory[chip8->I + i] & (0x01 << j)) {
                pixelValue = 0xFF;
            } else {
                pixelValue = 0x00;
            }
            if((chip8->video[pos + (7 - j)] ^ pixelValue) != (chip8->video[pos + (7 - j)] | pixelValue)) {
                erasedPixels = 0x01;
            }
            chip8->video[pos + (7 - j)] ^= pixelValue;
        }
    }
    if(erasedPixels) {
        chip8->V[0x0F] = 1;
    } else {
        chip8->V[0x0F] = 0;
    }
    chip8->PC += 2;
}

void skipVXKey(Chip8 *chip8) { //Opcode 24, 0xEx9E, SKP VX
    if(queryKey(chip8->V[chip8->opcodeNibble[1]])) {
        chip8->PC += 2;
    }
    chip8->PC += 2;
}

void skipNotVXKey(Chip8 *chip8) { //Opcode 25, 0xExA1, SKNP VX
    if(!queryKey(chip8->V[chip8->opcodeNibble[1]])) {
        chip8->PC += 2;
    }
    chip8->PC += 2;
}

void loadVXDT(Chip8 *chip8) { //Opcode 26, 0xFx07, LD VX, DT
    chip8->V[chip8->opcodeNibble[1]] = chip8->DT;
    chip8->PC += 2;
}

void loadVXK(Chip8 *chip8) { //Opcode 27, 0xFx0A, LD VX, K
    if(chip8->mostRecentKey != 0xFF) {
        chip8->V[chip8->opcodeNibble[1]] = chip8->mostRecentKey;
        chip8->mostRecentKey = 0xFF;
        chip8->PC += 2;
    } else {
        chip8->awaitKey = 1;
    }
}

void loadDTVX(Chip8 *chip8) { //Opcode 28, 0xFx15, LD DT, VX
    chip8->DT = chip8->V[chip8->opcodeNibble[1]];
    chip8->PC += 2;
}

void loadSTVX(Chip8 *chip8) { //Opcode 29, 0xFx18, LD ST, VX
    chip8->ST = chip8->V[chip8->opcodeNibble[1]];
    chip8->PC += 2;
}

void addIVX(Chip8 *chip8) { //Opcode 30, 0xFx1E, ADD I, VX
    chip8->I += chip8->V[chip8->opcodeNibble[1]];
    chip8->PC += 2;
}

void loadFontAddress (Chip8 *chip8) { //Opcode 31, 0xFx29, LD F, VX
    chip8->I = chip8->V[chip8->opcodeNibble[1]] * 5 + 50;
    chip8->PC += 2;
}

void loadBCDI(Chip8 *chip8) { //Opcode 32, 0xFx33, LD B, VX
    uint8_t digits[3];
    uint8_t num = chip8->V[chip8->opcodeNibble[1]];
    for(uint8_t i = 0; i < 3; i++) {
        digits[2 - i] = num % 10;
        num /= 10;
    }
    for(uint8_t i = 0; i < 3; i++) {
        chip8->memory[chip8->I + i] = digits[i];
    }
    chip8->PC += 2;
}

void loadMemoryVX(Chip8 *chip8) { //Opcode 33, 0xFx55, LD [I], VX
    for(uint8_t i = 0; i < chip8->opcodeNibble[1] + 1; i++) {
        chip8->memory[chip8->I + i] = chip8->V[i];
    }
    chip8->PC += 2;
}

void loadVXMemory(Chip8 *chip8) { //Opcode 34, 0xFx65, LD VX [I]
    for(uint8_t i = 0; i < chip8->opcodeNibble[1] + 1; i++) {
        chip8->V[i] = chip8->memory[chip8->I + i];
    }
    chip8->PC += 2;
}
