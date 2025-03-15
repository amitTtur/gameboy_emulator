#pragma once
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include "CPU.h"
#include "input.h"
#include <fstream>
#include <sstream>

class Gameboy { // will be expanded
public:
	Gameboy();
	~Gameboy();

	void init();

private:
	std::string _romPath;
	Memory _mem;
	PPU _ppu;
	CPU _cpu;
	Screen _sdlScreen;
	inputhandler _inputHandler;

	void run();
	bool readConfigFile(const std::string& filename, bool& debugMode, int& screenSize, bool& colorPalette, bool& saveProgress, std::string& saveFolderPath);
};