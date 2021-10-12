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

#include "chip8Type.h"
#include "chip8.h"
#include "opcode.h"

void loadNibbles(Chip8 *chip8);

void opcode0(Chip8 *chip8);
void opcode8(Chip8 *chip8);
void opcodeE(Chip8 *chip8);
void opcodeF(Chip8 *chip8);
void illegalInstruction(Chip8 *chip8);


int loadRom(Chip8 *chip8, FILE *rom) {
    fseek(rom, 0, SEEK_END);
    uint64_t size = ftell(rom);
    if(size > 3584) { //ROM too large to fit RAM
        return 1;
    }
    fseek(rom, 0, SEEK_SET);
    for(uint16_t i = 0; i < size; i++) {
        fread(&chip8->memory[0x200 + i], 1, 1, rom);
    }
    chip8->PC = 0x200;
    return 0;
}

void initializeSystem(Chip8 *chip8) {
    for(uint16_t i = 0; i < 4096; i++) {
        chip8->memory[i] = 0;
    }
    for(uint8_t i = 0; i < 16; i++) {
        chip8->stack[i] = 0;
        chip8->V[i] = 0;
    }
    clearScreen(chip8);

    chip8->DT = 0;
    chip8->ST = 0;
    chip8->SP = 0;
    chip8->I  = 0;
    chip8->PC = 0;

    uint8_t font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
    };
    
    for(uint8_t i = 0; i < 80; i++) {
        chip8->memory[i + 50] = font[i];
    }

    for(int i = 0; i < 16; i++) {
        chip8->opcodeTable[i] = illegalInstruction;
        chip8->opcode8Table[i] = illegalInstruction;
    }

    for(int i = 0; i < 256; i++) {
        chip8->opcode0Table[i] = illegalInstruction;
        chip8->opcodeETable[i] = illegalInstruction;
        chip8->opcodeFTable[i] = illegalInstruction;
    }

    chip8->opcodeTable[0x0] = opcode0;
    chip8->opcode0Table[0xE0] = clearScreen;
    chip8->opcode0Table[0xEE] = returnSubroutine;

    chip8->opcodeTable[0x1] = jumpAdress; //Opcode 3 1nnn, JP
    chip8->opcodeTable[0x2] = callSubroutine; //Opcode 4 2nnn, CALL
    chip8->opcodeTable[0x3] = skipVXEqualsNum; //Opcode 5 3xkk, SE VX
    chip8->opcodeTable[0x4] = skipVXNotNum; //Opcode 6 4xkk, SNE VC
    chip8->opcodeTable[0x5] = skipVXEqualsVY; //Opcode 7 5xy0, SE VX, VY
    chip8->opcodeTable[0x6] = loadVXByte; //Opcode 8 6xkk, LD VX
    chip8->opcodeTable[0x7] = addVXByte; //Opcode 9 7xnn, ADD VX
    
    chip8->opcodeTable[0x8] = opcode8;
    chip8->opcode8Table[0x0] = loadVXVY; //Opcode 10 8xy0, LD VX, VY
    chip8->opcode8Table[0x1] = bitOrVXVY; //Opcode 11 8xy1, OR VX, XY
    chip8->opcode8Table[0x2] = bitAndVXVY; //Opcode 12 8xy2, AND VX, VY
    chip8->opcode8Table[0x3] = bitXorVXVY; //Opcode 13 8xy3, XOR VX, VY
    chip8->opcode8Table[0x4] = addVXVY; //Opcode 14 8xy4, ADD VX, VY
    chip8->opcode8Table[0x5] = subtractVYVX; //Opcode 15 8xy5, SUB VX, VY
    chip8->opcode8Table[0x6] = rightShiftVX; //Opcode 16Q 8xy6, SHR VX
    chip8->opcode8Table[0x7] = subtractVXVY; //Opcode 17 8xy7, SUBN VX, VY
    chip8->opcode8Table[0xE] = leftShiftVX; //Opcode 18Q 8xyE, SHL VX

    chip8->opcodeTable[0x9] = skipVXNotEqualVY; //Opcode 19 9xy0, SNE VX, VY
    chip8->opcodeTable[0xA] = loadIAddress; //Opcode 20 Annn, LD I
    chip8->opcodeTable[0xB] = jumpPlusV0; //Opcode 21 Bnnn, JP V0
    chip8->opcodeTable[0xC] = generateRandom; //Opcode 22 Cxkk, RND VX
    chip8->opcodeTable[0xD] = draw; //Opcode 23 Dxyn, DRW VX, VY
    
    chip8->opcodeTable[0xE] = opcodeE;
    chip8->opcodeETable[0x9E] = skipVXKey; //Opcode 24 Ex9E, SKP VX
    chip8->opcodeETable[0xA1] = skipNotVXKey; //Opcode 25 ExA1, SKNP VX

    chip8->opcodeTable[0xF] = opcodeF;
    chip8->opcodeFTable[0x07] = loadVXDT; //Opcode 26 Fx07, LD VX, DT
    chip8->opcodeFTable[0x0A] = loadVXK; //Opcode 27 Fx0A, LD VX, K
    chip8->opcodeFTable[0x15] = loadDTVX; //Opcode 28 Fx15, LD DT, VX
    chip8->opcodeFTable[0x18] = loadSTVX; //Opcode 29 Fx18, LD ST, VX
    chip8->opcodeFTable[0x1E] = addIVX; //Opcode 30 Fx1E, ADD I, VX
    chip8->opcodeFTable[0x29] = loadFontAddress; //Opcode 30 Fx29, LD F, VX
    chip8->opcodeFTable[0x33] = loadBCDI; //Opcode 31 Fx33, LD B, VX
    chip8->opcodeFTable[0x55] = loadMemoryVX; //Opcode 32 Fx55, LD [I], VX
    chip8->opcodeFTable[0x65] = loadVXMemory; //Opcode 33 Fx65, LD VX [I]
    
    if(chip8->quirkFlags & 0x01) {
        chip8->opcode8Table[0x6] = rightShiftVXVY;
        chip8->opcode8Table[0xE] = leftShiftVXVY;
    }

    chip8->illegalAccess = 0;
    chip8->awaitKey = 0;
    chip8->mostRecentKey = 0xFF;
}

int readInstruction(Chip8 *chip8) {
    loadNibbles(chip8);
    chip8->opcodeTable[chip8->opcodeNibble[0]](chip8);
    printf("Instruction %x %x\n", chip8->memory[chip8->PC], chip8->memory[chip8->PC + 1]);
    return chip8->illegalAccess;
}

void loadNibbles(Chip8 *chip8) {
    chip8->opcodeNibble[0] = (chip8->memory[chip8->PC] & 0xF0) / 16;
    chip8->opcodeNibble[1] = chip8->memory[chip8->PC] & 0x0F;
    chip8->opcodeNibble[2] = (chip8->memory[chip8->PC + 1] & 0xF0) / 16;
    chip8->opcodeNibble[3] = chip8->memory[chip8->PC + 1] & 0x0F;
}

void opcode0(Chip8 *chip8) {
    chip8->opcode0Table[chip8->memory[chip8->PC + 1]](chip8);
}

void opcode8(Chip8 *chip8) {
    chip8->opcode8Table[chip8->opcodeNibble[3]](chip8);
}

void opcodeE(Chip8 *chip8) {
    chip8->opcodeETable[chip8->memory[chip8->PC + 1]](chip8);
}

void opcodeF(Chip8 *chip8) {
    chip8->opcodeFTable[chip8->memory[chip8->PC + 1]](chip8);
}

void illegalInstruction(Chip8 *chip8) {
    printf("Illegal instruction or unknown native code jump encounted, aborting!\n");
    chip8->illegalAccess = 1;
}
