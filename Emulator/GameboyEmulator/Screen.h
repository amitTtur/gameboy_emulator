#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include<cstdint>
#include "CodesAndDefines.h"

#ifndef SCREENH
#define SCREENH

class Screen
{
public:
	void makeScreen();
	void updateScreen(uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT]);
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

	uint8_t frameBufferBefore[SCREEN_WIDTH * SCREEN_HEIGHT];
	uint8_t frameBufferAfter[SCREEN_WIDTH * SCREEN_HEIGHT];
};

#endif // !SCREENH
