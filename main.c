#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "chip8Type.h"
#include "chip8.h"
#include "sdlFunctions.h"

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
#endif


int main(int argc, char *argv[]) {
    //Ensures a rom name is specified
    if(argc != 2) {
        printf("Invalid number of command line arguments!\nUsage: chip8 [Path to ROM]\n");
        return 1;
    }
    //Opens a file pointer to the location specified and reads it as binary
    FILE *rom = fopen(argv[1], "rb");
    if(rom == NULL) {
        printf("Error: File specified does not exist!\nUsage: chip [Path to ROM]\n");
        return 2;
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
    int i = 0;
    int j;
    clock_t freq;
    for(uint8_t i = 0; i < 32; i++) { //Clear screen
        for(uint8_t j = 0; j < 64; j++) {
            chip8.video[(i * 64) + j] = 0;
        }
    }
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    graphicsInit(&window, &renderer, &texture, 16);
    while(1) {
        if(queryKey(0x10)) {
            break;
        }
        freq = clock();
        for(j = 0; j < 8; j++) {
            error = readInstruction(&chip8);
            if(error) {
                printf("ERROR\n");
                return 4;
            }
            i++;
        }
        if(chip8.ST) {
            chip8.ST--;
        }
        if(chip8.DT) {
            chip8.DT--;
        }
        updateFrame(&chip8, window, renderer, texture, 16);
        freq = clock() - freq;
        if((double) freq / CLOCKS_PER_SEC < 1 / 60.0) {
            usleep(((1.0 / 60.0) - ((double) freq / CLOCKS_PER_SEC)) * 1000000);
        }
    }
    graphicsEnd(&window, &renderer, &texture);
    return 0;
}
