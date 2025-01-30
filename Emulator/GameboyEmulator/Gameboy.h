#pragma once
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include "CPU.h"
#include "Screen.h"
#include "PPU.h"


class Gameboy { // will be expanded
public:
	Gameboy(const std::string romPath);

	void run();

private:
	const std::string _romPath;
	CPU _cpu;
	PPU _ppu;
	Screen _sdlScreen;
};