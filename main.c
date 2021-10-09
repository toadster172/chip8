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
