#include "chip8Type.h"
#include "chip8.h"
#include "opcode.h"

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
}

int readInstruction(Chip8 *chip8) {
    //Opcode 1 00E0, CLS, clear display
    if(chip8->memory[chip8->PC] == 0x00 && chip8->memory[chip8->PC + 1] == 0xE0) {
        clearScreen(chip8);
    } else  //Opcode 2 00EE, RET, return subroutine
    if(chip8->memory[chip8->PC] == 0x00 && chip8->memory[chip8->PC + 1] == 0xEE) {
        returnSubroutine(chip8);
    } else  //Opcode 3 1nnn, JP, jump address
    if((chip8->memory[chip8->PC] & 0xF0) == 0x10) {
        jumpAdress(chip8);
    } else  //Opcode 4 2nnn, CALL, call subroutine
    if((chip8->memory[chip8->PC] & 0xF0) == 0x20) {
        callSubroutine(chip8);
    } else  //Opcode 5 3xkk, SE VX, jump 2 if vx is k
    if((chip8->memory[chip8->PC] & 0xF0) == 0x30) {
        skipVXEqualsNum(chip8);
    } else  //Opcode 6 4xkk, SNE VC, jump 2 if vx not k
    if((chip8->memory[chip8->PC] & 0xF0) == 0x40) {
        skipVXNotNum(chip8);
    } else  //Opcode 7 5xy0, SE VX, VY, jump 2 if vx is vy
    if((chip8->memory[chip8->PC] & 0xF0) == 0x50) {
        skipVXEqualsVY(chip8);
    } else  //Opcode 8 6xkk, LD VX, store k in vx
    if((chip8->memory[chip8->PC] & 0xF0) == 0x60) {
        loadVXByte(chip8);
    } else  //Opcode 9 7xnn, ADD VX, add n to vx
    if((chip8->memory[chip8->PC] & 0xF0) == 0x70) {
        addVXByte(chip8);
    } else  //Opcode 10 8xy0, LD VX, VY, store VY in VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x00) {
        loadVXVY(chip8);
    } else  //Opcode 11 8xy1, OR VX, XY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x01) {
        bitOrVXVY(chip8);
    } else  //Opcode 12 8xy2, AND VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x02) {
        bitAndVXVY(chip8);
    } else  //Opcode 13 8xy3, XOR VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x03) {
        bitXorVXVY(chip8);
    } else  //Opcode 14 8xy4, ADD VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x04) {
        addVXVY(chip8);
    } else  //Opcode 15 8xy5, SUB VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x05) {
        subtractVYVX(chip8);
    } else  //Opcode 16 8xy6, SHR VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x06) {
        rightShiftVY(chip8);
    } else  //Opcode 17 8xy7, SUBN VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x07) {
        subtractVXVY(chip8);
    } else  //Opcode 18 8xyE, SHL VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x80 && (chip8->memory[chip8->PC + 1] & 0x0F) == 0x0E) {
        leftShiftVY(chip8);
    } else  //Opcode 19 9xy0, SNE VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0x90) {
        skipVXNotEqualVY(chip8);
    } else  //Opcode 20 Annn, LD I
    if((chip8->memory[chip8->PC] & 0xF0) == 0xA0) {
        loadIAddress(chip8);
    } else  //Opcode 21 Bnnn, JP V0
    if((chip8->memory[chip8->PC] & 0xF0) == 0xB0) {
        jumpPlusV0(chip8);
    } else  //Opcode 22 Cxkk, RND VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xC0) {
        generateRandom(chip8);
    } else  //Opcode 23 Dxyn, DRW VX, VY
    if((chip8->memory[chip8->PC] & 0xF0) == 0xD0) {
        draw(chip8);
    } else  //Opcode 24 Ex9E, SKP VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xE0 && chip8->memory[chip8->PC + 1] == 0x9E) {
        skipVXKey(chip8);
    } else  //Opcode 25 ExA1, SKNP VX
    if((chip8->memory[chip8->PC] & 0xE0) == 0xE0 && chip8->memory[chip8->PC + 1] == 0xA1) {
        skipNotVXKey(chip8);
    } else  //Opcode 26 Fx07, LD VX, DT
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x07) {
        loadVXDT(chip8);
    } else  //Opcode 27 Fx0A, LD VX, K
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x0A) {
        loadVXK(chip8);
    } else  //Opcode 28 Fx15, LD DT, VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x15) {
        loadDTVX(chip8);
    } else  //Opcode 29 Fx18, LD ST, VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x18) {
        loadSTVX(chip8);
    } else  //Opcode 30 Fx1E, ADD I, VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x1E) {
        addIVX(chip8);
    } else  //Opcode 30 Fx29, LD F, VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x29) {
        loadFontAddress(chip8);
    } else  //Opcode 31 Fx33, LD B, VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x33) {
        loadBCDI(chip8);
    } else  //Opcode 32 Fx55, LD [I], VX
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x55) {
        loadMemoryVX(chip8);
    } else  //Opcode 33 Fx65, LD VX [I]
    if((chip8->memory[chip8->PC] & 0xF0) == 0xF0 && chip8->memory[chip8->PC + 1] == 0x65) {
        loadVXMemory(chip8);
    } else {
        if((chip8->memory[chip8->PC] & 0xF0) == 0x00) {
            printf("Machine code ignored\n");
            chip8->PC += 2;
            return 0;
        }
        printf("Unknown instruction %x %x\n", chip8->memory[chip8->PC], chip8->memory[chip8->PC + 1]);
        return 1;
    }
    printf("Instruction %x %x, %x\n", chip8->memory[chip8->PC],
     chip8->memory[chip8->PC + 1], chip8->memory[chip8->PC] & 0xF0);
    return 0;
}
