#include "chip8Type.h"
#include "chip8.h"
#include "opcode.h"

void loadNibbles(Chip8 *chip8);

void opcode0(Chip8 *chip8);
void opcode8(Chip8 *chip8);
void opcodeE(Chip8 *chip8);
void opcodeF(Chip8 *chip8);


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
    for(uint16_t i = 0; i < 256; i++) {
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

    chip8->opcodeTable[0x0] = opcode0;
    chip8->opcodeTable[0x1] = jumpAdress; //Opcode 3 1nnn, JP
    chip8->opcodeTable[0x2] = callSubroutine; //Opcode 4 2nnn, CALL
    chip8->opcodeTable[0x3] = skipVXEqualsNum; //Opcode 5 3xkk, SE VX
    chip8->opcodeTable[0x4] = skipVXNotNum; //Opcode 6 4xkk, SNE VC
    chip8->opcodeTable[0x5] = skipVXEqualsVY; //Opcode 7 5xy0, SE VX, VY
    chip8->opcodeTable[0x6] = loadVXByte; //Opcode 8 6xkk, LD VX
    chip8->opcodeTable[0x7] = addVXByte; //Opcode 9 7xnn, ADD VX
    chip8->opcodeTable[0x8] = opcode8;
    chip8->opcodeTable[0x9] = skipVXNotEqualVY; //Opcode 19 9xy0, SNE VX, VY
    chip8->opcodeTable[0xA] = loadIAddress; //Opcode 20 Annn, LD I
    chip8->opcodeTable[0xB] = jumpPlusV0; //Opcode 21 Bnnn, JP V0
    chip8->opcodeTable[0xC] = generateRandom; //Opcode 22 Cxkk, RND VX
    chip8->opcodeTable[0xD] = draw; //Opcode 23 Dxyn, DRW VX, VY
    chip8->opcodeTable[0xE] = opcodeE;
    chip8->opcodeTable[0xF] = opcodeF;
}

int readInstruction(Chip8 *chip8) {
    loadNibbles(chip8);
    chip8->opcodeTable[chip8->opcodeNibble[0]](chip8);
    printf("Instruction %x %x\n", chip8->memory[chip8->PC], chip8->memory[chip8->PC + 1]);
    return 0;
}

void loadNibbles(Chip8 *chip8) {
    chip8->opcodeNibble[0] = (chip8->memory[chip8->PC] & 0xF0) / 16;
    chip8->opcodeNibble[1] = chip8->memory[chip8->PC] & 0x0F;
    chip8->opcodeNibble[2] = (chip8->memory[chip8->PC + 1] & 0xF0) / 16;
    chip8->opcodeNibble[3] = chip8->memory[chip8->PC + 1] & 0x0F;
}

void opcode0(Chip8 *chip8) {
    switch(chip8->opcodeNibble[3]) {
        case 0x0: 
            clearScreen(chip8); //Opcode 1 00E0, CLS, clear display
            return;
        case 0xE:
            returnSubroutine(chip8); //Opcode 2 00EE, RET, return subroutine
            return;
        default:
            printf("Machine code skipped!\n");
            chip8->PC += 2;
            return;
    }
}

void opcode8(Chip8 *chip8) {
    switch(chip8->opcodeNibble[3]) {
        case 0x0:
            loadVXVY(chip8); //Opcode 10 8xy0, LD VX, VY
            return;
        case 0x1:
            bitOrVXVY(chip8); //Opcode 11 8xy1, OR VX, XY
            return;
        case 0x2:
            bitAndVXVY(chip8); //Opcode 12 8xy2, AND VX, VY
            return;
        case 0x3:
            bitXorVXVY(chip8); //Opcode 13 8xy3, XOR VX, VY
            return;
        case 0x4:
            addVXVY(chip8); //Opcode 14 8xy4, ADD VX, VY
            return;
        case 0x5:
            subtractVYVX(chip8); //Opcode 15 8xy5, SUB VX, VY
            return;
        case 0x6:
            rightShiftVY(chip8); //Opcode 16 8xy6, SHR VX, VY
            return;
        case 0x7:
            subtractVXVY(chip8); //Opcode 17 8xy7, SUBN VX, VY
            return;
        case 0xE:
            leftShiftVY(chip8); //Opcode 18 8xyE, SHL VX, VY
            return;
    }
}

void opcodeE(Chip8 *chip8) {
    switch(chip8->opcodeNibble[3]) {
        case 0xE:
            skipVXKey(chip8); //Opcode 24 Ex9E, SKP VX
            return;
        case 0x1:
            skipNotVXKey(chip8); //Opcode 25 ExA1, SKNP VX
            return;
    }
}

void opcodeF(Chip8 *chip8) {
    switch(chip8->memory[chip8->PC + 1]) {
        case 0x07: 
            loadVXDT(chip8); //Opcode 26 Fx07, LD VX, DT
            return;
        case 0x0A:
            loadVXK(chip8); //Opcode 27 Fx0A, LD VX, K
            return;
        case 0x15:
            loadDTVX(chip8); //Opcode 28 Fx15, LD DT, VX
            return;
        case 0x18:
            loadSTVX(chip8); //Opcode 29 Fx18, LD ST, VX
            return;
        case 0x1E:
            addIVX(chip8); //Opcode 30 Fx1E, ADD I, VX
            return;
        case 0x29:
            loadFontAddress(chip8); //Opcode 30 Fx29, LD F, VX
            return;
        case 0x33:
            loadBCDI(chip8); //Opcode 31 Fx33, LD B, VX
            return;
        case 0x55:
            loadMemoryVX(chip8); //Opcode 32 Fx55, LD [I], VX
            return;
        case 0x65:
            loadVXMemory(chip8); //Opcode 33 Fx65, LD VX [I]
            return;
    }
}
