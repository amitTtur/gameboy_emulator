#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include "CodesAndDefines.h"

#ifndef SCREENH
#define SCREENH

class Screen
{
public:
	//inits the screen
	void makeScreen(int pixelToScreen, bool palleteSwap);

	//updates the screen
	void updateScreen(uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT]);

	SDL_Window* getWindow();
	SDL_Renderer* getRenderer();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	uint8_t frameBufferBefore[SCREEN_WIDTH * SCREEN_HEIGHT];
	uint8_t frameBufferAfter[SCREEN_WIDTH * SCREEN_HEIGHT];
	bool _greenPallete;
};

#endif // !SCREENH
