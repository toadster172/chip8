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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "chip8Type.h"
#include "chip8.h"
#include "sdlFunctions.h"

int main(int argc, char *argv[]) {
    //Ensures a rom name is specified
    if(argc == 1) {
        printf("Invalid number of command line arguments!\nUsage: chip8 [Path to ROM] (Instructions per frame)\n");
        return 1;
    }
    //Opens a file pointer to the location specified and reads it as binary
    FILE *rom = fopen(argv[1], "rb");
    if(rom == NULL) {
        printf("Error: File specified does not exist!\nUsage: chip8 [Path to ROM] (Instructions per frame)\n");
        return 2;
    }
    int instructionsPerFrame = 8;
    if(argc == 3) {
        instructionsPerFrame = atoi(argv[2]);
        if(!instructionsPerFrame) {
            printf("Error: Invalid frequency!\n Usage: chip8 [Path to ROM] (Instructions per frame)\n");
        }
    }

    Chip8 chip8;
    if(argc == 4) {
        chip8.quirkFlags = atoi(argv[3]);
    } else {
        chip8.quirkFlags = 0;
    }

    initializeSystem(&chip8);
    int error = loadRom(&chip8, rom);
    fclose(rom);
    if(error == 1) { //loadRom returns 1 if ROM is too large to fit RAM
        printf("ROM size too large!\n");
        return 3;
    }
    srand(time(NULL));
    clock_t freq;
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    graphicsInit(&window, &renderer, &texture, 16);
    audioInit();

    while(1) {
        if(queryKey(0x10)) {
            break;
        }
        freq = clock();
        for(int i = 0; i < instructionsPerFrame; i++) {
            if(chip8.awaitKey) {
                uint8_t key = getNewKey();
                if(key != 0xFF) {
                    chip8.mostRecentKey = key;
                    chip8.awaitKey = 0;
                }
                break;
            }
            error = readInstruction(&chip8);
            if(error) {
                printf("ERROR\n");
                return 4;
            }
        }

        if(chip8.ST) {
            SDL_PauseAudio(0);
            chip8.ST--;
            if(!chip8.ST) {
                SDL_PauseAudio(1);
            }
        }
        if(chip8.DT) {
            chip8.DT--;
        }

        updateFrame(&chip8, window, renderer, texture, 16);
        freq = clock() - freq;
        if((double) freq / CLOCKS_PER_SEC < 1 / 60.0) {
            SDL_Delay(((1.0 / 60.0) - ((double) freq / CLOCKS_PER_SEC)) * 1000);
        }
    }
    graphicsEnd(&window, &renderer, &texture);
    return 0;
}
