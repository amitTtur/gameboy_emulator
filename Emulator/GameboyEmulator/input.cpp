#include "input.h"
#include <bitset>

inputhandler::inputhandler(Memory& memRef) : _mem(memRef), _inputReg(_mem[INPUT_REG_LOC])
{
	_inputReg = 0xcf;
	_padState = 0;
}

/*
A - 0
B - 1
SELECT - 2
START - 3
RIGHT - 4
LEFT - 5
UP - 6
DOWN - 7
*/

void inputhandler::handleInputs(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_a:
			updateState(5, 1);
			break;

		case SDLK_w:
			updateState(6, 1);
			break;

		case SDLK_d:
			updateState(4, 1);
			break;

		case SDLK_s:
			updateState(7, 1);
			break;

		case SDLK_c:
			updateState(3, 1);
			break;

		case SDLK_z:
			updateState(2, 1);
			break;

		case SDLK_k:
			updateState(1, 1);
			break;

		case SDLK_l:
			updateState(0, 1);
			break;
		default:
			break;
		}
		debugInputs();
	}
	else if (event.type == SDL_KEYUP)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_a:
			updateState(5, 0);
			break;

		case SDLK_w:
			updateState(6, 0);
			break;

		case SDLK_d:
			updateState(4, 0);
			break;

		case SDLK_s:
			updateState(7, 0);
			break;

		case SDLK_c:
			updateState(3, 0);
			break;

		case SDLK_z:
			updateState(2, 0);
			break;

		case SDLK_k:
			updateState(1, 0);
			break;

		case SDLK_l:
			updateState(0, 0);
			break;
		default:
			break;
		}
		debugInputs();
	}
}

void inputhandler::debugInputs()
{
	std::cout << 
		"inputreg: " << std::bitset<8>(_inputReg) << std::endl <<
		"pad state: " << std::bitset<8>(_padState) <<
		std::endl << std::endl;
}

void inputhandler::setBit(uint8_t bit, uint8_t val)
{
	_inputReg = (_inputReg & ~(1 << bit)) | (val << bit);
}

bool inputhandler::getBit(uint8_t bit)
{
	return (_inputReg >> bit) & 1;
}

void inputhandler::updateState(uint8_t bit, uint8_t val)
{
	_padState = (_padState & ~(1 << bit)) | (val << bit);
	_mem.int_Joypad(1);

	_inputReg = ((~((_padState & 0x0f) | ((_padState >> 4) & 0x0f))) & 0x0f) | (_inputReg & 0xf0);
}

