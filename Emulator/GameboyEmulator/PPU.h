#pragma once
#ifndef PPU_H
#define PPU_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include <array>

#include "CodesAndDefines.h"
#include "PPURegisters.h"
#include "Memory.h"
#include "globalVars.h"
#include "Screen.h"

class PPU
{
public:
	PPU(Memory& _memRef, Screen& sdl);
	~PPU();

	void update(uint8_t cycles);

	void resetFrameBuffer();
	uint8_t(&getFrameBuffer())[SCREEN_WIDTH * SCREEN_HEIGHT];

private:

	//draws one line of thr background layer
	void drawBackground();

	//draws one line of the window layer
	void drawWindow();

	//draws one line of the sprites layer
	void drawSprites();

	//oam search - find sprites for current line
	void performOamSearch(std::vector<spriteSave>& spriteVector);

	//testing and debugging functions
	std::ofstream logFile;
	void writeCurrentStateToLogFile(std::vector<spriteSave> vector);


	uint8_t _frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
	Screen& _sdlScreen;
	Memory& _mem;
	REG_STAT _statReg;
	REG_LCDC _lcdcReg;
	uint16_t _allocatedCycles;
	uint8_t& _ly;
	uint8_t& _lyc;
	uint8_t& _scx;
	uint8_t& _scy;
	uint8_t& _wx;
	uint8_t& _wy;
};

#endif // !