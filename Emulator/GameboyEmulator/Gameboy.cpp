#include "Gameboy.h"

Gameboy::Gameboy(const std::string romPath) : _cpu(romPath), _romPath(romPath)
{
	_sdlScreen.makeScreen();
	globalVars::running(true);
}

void Gameboy::run()
{
	Uint32 prev = 0;
	Uint32 current = 0;
	Uint32 deltaTime = 0;
	float fps = 0;

	//create sdl screen

	while (globalVars::running())
	{
		current = SDL_GetTicks();
		deltaTime = current - prev;

		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			if (SDL_QUIT == ev.type)
			{
				globalVars::running(false);
				break;
			}

			if (ev.type == SDL_KEYDOWN)
			{
				switch (ev.key.keysym.sym)
				{
				case SDLK_a:
					std::cout << "a key down" << std::endl;
					break;
				default:
					break;
				}
			}

			if (ev.type == SDL_KEYUP)
			{
				switch (ev.key.keysym.sym)
				{
				case SDLK_a:
					std::cout << "a key up" << std::endl;
					break;
				default:
					break;
				}
			}
		}


		if (deltaTime > 1000.0/SYS_FPS) //59.7 fps loop
		{
			fps = 1000.0 / deltaTime;
			std::cout << fps << std::endl;

			prev = current;

			//checking for input
			

			//update gameboy
			_cpu.update();

			_ppu.update();

			//update sdl screen
			//std::cout << "turgi is annoying" << std::endl;
			_sdlScreen.updateScreen(_ppu.getFrameBuffer());
		}

	}
}