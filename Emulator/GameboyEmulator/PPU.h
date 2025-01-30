#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include<cstdint>
#include "CodesAndDefines.h"

class PPU
{
public:
	PPU();
	~PPU();

	void update();

	void resetFrameBuffer();
	uint8_t(&getFrameBuffer())[SCREEN_WIDTH * SCREEN_HEIGHT];

private:
	int current; //for testing
	uint8_t _frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
};