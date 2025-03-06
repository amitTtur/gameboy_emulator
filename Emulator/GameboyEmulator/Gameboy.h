#pragma once
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include "CPU.h"
#include "input.h"


class Gameboy { // will be expanded
public:
	Gameboy(const std::string romPath);
	~Gameboy();

	void run();

private:
	const std::string _romPath;
	Memory _mem;
	PPU _ppu;
	CPU _cpu;
	Screen _sdlScreen;
	inputhandler _inputHandler;
};