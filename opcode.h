#ifndef OPCODE_H
#define OPCODE_H

#include "chip8Type.h"

void clearScreen     (Chip8 *chip8); //Opcode 1 , 0x00E0, CLS
void returnSubroutine(Chip8 *chip8); //Opcode 2 , 0x00EE, RET
void jumpAdress      (Chip8 *chip8); //Opcode 3 , 0x1nnn, JP
void callSubroutine  (Chip8 *chip8); //Opcode 4 , 0x2nnn, CALL
void skipVXEqualsNum (Chip8 *chip8); //Opcode 5 , 0x3xkk, SE VX
void skipVXNotNum    (Chip8 *chip8); //Opcode 6 , 0x4xkk, SNE VC
void skipVXEqualsVY  (Chip8 *chip8); //Opcode 7 , 0x5xy0, SE VX, VY
void loadVXByte      (Chip8 *chip8); //Opcode 8 , 0x6xkk, LD VX
void addVXByte       (Chip8 *chip8); //Opcode 9 , 0x7xnn, ADD VX
void loadVXVY        (Chip8 *chip8); //Opcode 10, 0x8xy0, LD VX, VY
void bitOrVXVY       (Chip8 *chip8); //Opcode 11, 0x8xy1, OR VX, XY
void bitAndVXVY      (Chip8 *chip8); //Opcode 12, 0x8xy2, AND VX, VY
void bitXorVXVY      (Chip8 *chip8); //Opcode 13, 0x8xy3, XOR VX, VY
void addVXVY         (Chip8 *chip8); //Opcode 14, 0x8xy4, ADD VX, VY
void subtractVYVX    (Chip8 *chip8); //Opcode 15, 0x8xy5, SUB VX, VY
void rightShiftVXVY  (Chip8 *chip8); //Opcode 16, 8xy6, SHR VX, VY
void rightShiftVX    (Chip8 *chip8); //Opcode 16Q, 0x8xy6, SHR VX
void subtractVXVY    (Chip8 *chip8); //Opcode 17, 0x8xy7, SUBN VX, VY
void leftShiftVXVY   (Chip8 *chip8); //Opcode 18, 0x8xyE, SHL VX, VY
void leftShiftVX     (Chip8 *chip8); //Opcode 18Q, 0x8xyE, SHL VX
void skipVXNotEqualVY(Chip8 *chip8); //Opcode 19, 0x9xy0, SNE VX, VY
void loadIAddress    (Chip8 *chip8); //Opcode 20, 0xAnnn, LD I
void jumpPlusV0      (Chip8 *chip8); //Opcode 21, 0xBnnn, JP V0
void generateRandom  (Chip8 *chip8); //Opcode 22, 0xCxkk, RND VX
void draw            (Chip8 *chip8); //Opcode 23, 0xDxyn, DRW VX, VY
void skipVXKey       (Chip8 *chip8); //Opcode 24, 0xEx9E, SKP VX
void skipNotVXKey    (Chip8 *chip8); //Opcode 25, 0xExA1, SKNP VX
void loadVXDT        (Chip8 *chip8); //Opcode 26, 0xFx07, LD VX, DT
void loadVXK         (Chip8 *chip8); //Opcode 27, 0xFx0A, LD VX, K
void loadDTVX        (Chip8 *chip8); //Opcode 28, 0xFx15, LD DT, VX
void loadSTVX        (Chip8 *chip8); //Opcode 29, 0xFx18, LD ST, VX
void addIVX          (Chip8 *chip8); //Opcode 30, 0xFx1E, ADD I, VX
void loadFontAddress (Chip8 *chip8); //Opcode 30, 0xFx29, LD F, I
void loadBCDI        (Chip8 *chip8); //Opcode 31, 0xFx33, LD B, VX
void loadMemoryVX    (Chip8 *chip8); //Opcode 32, 0xFx55, LD [I], VX
void loadVXMemory    (Chip8 *chip8); //Opcode 33, 0xFx65, LD VX [I]

#endif
