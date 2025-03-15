#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <iostream>

#include "Memory.h"
#include "globalVars.h"

class inputhandler
{
public:
	inputhandler(Memory& memRef);

	void handleInputs(SDL_Event& event);

private:
	Memory& _mem;
	uint8_t& _inputReg;
	
	//bitwise functions for the input register
	void setBit(uint8_t bit, uint8_t val);
	bool getBit(uint8_t bit);
	

	/*
	update pad state
	A - 0
	B - 1
	SELECT - 2
	START - 3
	RIGHT - 4
	LEFT - 5
	UP - 7
	DOWN - 8
	*/
	void updateState(uint8_t bit, uint8_t val);

	//debugging
	void debugInputs();

};