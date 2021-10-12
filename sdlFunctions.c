#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL.h>

#include "sdlFunctions.h"
#include "chip8Type.h"

void graphicsInit(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture, uint8_t scale) {
    SDL_Init(SDL_INIT_VIDEO);
    *window = SDL_CreateWindow("Chip 8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * scale, 32 * scale, SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    SDL_SetRenderDrawColor(*renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_Delay(1000 / 60);
    SDL_RenderClear(*renderer);
    SDL_RenderPresent(*renderer);
}

//Handles the beeper audio with 441 hz square wave
void squareCallback(void *userdata, Uint8 *stream, int len) {
    int location = *((int *) userdata);

    for(int i = 0; i < len; i++, location++) {
        if(location % 100 < 50) {
            stream[i] = 64;
        } else {
            stream[i] = -64;
        }
    }

    location %= 100;
    *((int *) userdata) = location;
}

void audioInit() {
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec desiredAudio;
    desiredAudio.freq = 44100;
    desiredAudio.format = AUDIO_S8;
    desiredAudio.channels = 1;
    desiredAudio.samples = 4096;
    desiredAudio.callback = squareCallback;
    desiredAudio.userdata = malloc(sizeof(int));

    SDL_AudioSpec returnedAudio;
    SDL_OpenAudio(&desiredAudio, &returnedAudio);
}

void updateFrame(Chip8 *chip8, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture, uint8_t scale) {
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, NULL, chip8->video, 64);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

uint8_t queryKey(uint8_t key) {
    const uint8_t *keyboard = SDL_GetKeyboardState(NULL);
    SDL_PumpEvents();
    switch(key) {
        case 0xA: 
            return keyboard[SDL_SCANCODE_Z];
        case 0x0:
            return keyboard[SDL_SCANCODE_X];
        case 0xB:
            return keyboard[SDL_SCANCODE_C];
        case 0xF:
            return keyboard[SDL_SCANCODE_V];
        case 0x7:
            return keyboard[SDL_SCANCODE_A];
        case 0x8:
            return keyboard[SDL_SCANCODE_S];
        case 0x9:
            return keyboard[SDL_SCANCODE_D];
        case 0xE:
            return keyboard[SDL_SCANCODE_F];
        case 0x4:
            return keyboard[SDL_SCANCODE_Q];
        case 0x5:
            return keyboard[SDL_SCANCODE_W];
        case 0x6:
            return keyboard[SDL_SCANCODE_E];
        case 0xD:
            return keyboard[SDL_SCANCODE_R];
        case 0x1:
            return keyboard[SDL_SCANCODE_1];
        case 0x2:
            return keyboard[SDL_SCANCODE_2];
        case 0x3:
            return keyboard[SDL_SCANCODE_3];
        case 0xC:
            return keyboard[SDL_SCANCODE_4];
        case 0x10:
            return keyboard[SDL_SCANCODE_ESCAPE];
    }
    return 0;
}

void graphicsEnd(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture) {
    SDL_DestroyTexture(*texture);
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
    SDL_Quit();
}
