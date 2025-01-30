#include "PPU.h"

PPU::PPU()
{
	resetFrameBuffer();
	current = 0;
}

PPU::~PPU()
{
}

void PPU::update()
{
	_frameBuffer[current % (SCREEN_HEIGHT * SCREEN_WIDTH)] = 3;
	current++;
}

void PPU::resetFrameBuffer()
{
	for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++)
	{
		_frameBuffer[i] = 0;
	}
}

uint8_t(&PPU::getFrameBuffer())[SCREEN_WIDTH * SCREEN_HEIGHT]
{
	return _frameBuffer;
}
