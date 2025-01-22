#pragma once
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include "CPU.h"


class Gameboy { // will be expanded
public:
	Gameboy(const std::string romPath) : _running(true), _cpu(romPath), _romPath(romPath) {}

	void run();

private:
	const std::string _romPath;
	bool _running;
	CPU _cpu;
};