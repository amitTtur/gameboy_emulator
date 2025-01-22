#include "Gameboy.h"

void Gameboy::run()
{
	Uint32 prev = 0;
	Uint32 current = 0;
	Uint32 deltaTime = 0;

	//create sdl screen

	while (_running)
	{
		current = SDL_GetTicks();
		deltaTime = current - prev;

		//read input thrugh sdl

		if (deltaTime > 1000.0/SYS_FPS) //59.7 fps loop
		{
			//std::cout << "im running at " << 1000.0 / deltaTime << " frames per second!" << std::endl;

			prev = current;

			//update gameboy
			_cpu.update();

			//update sdl screen
		}
	}
}