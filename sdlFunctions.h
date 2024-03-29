#ifndef SDLFUNCTIONS_H
#define SDLFUNCTIONS_H

#include <stdint.h>
#include <SDL.h>
#include "chip8Type.h"

void graphicsInit(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture, uint8_t scale);
void audioInit(void);
void updateFrame(Chip8 *chip8, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture, uint8_t scale);
uint8_t queryKey(uint8_t key);
uint8_t getNewKey(void);
void graphicsEnd(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture);

#endif
