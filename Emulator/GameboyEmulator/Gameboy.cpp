#include "Gameboy.h"

Gameboy::Gameboy(const std::string romPath) : _cpu(_ppu, _mem), _ppu(_mem, _sdlScreen),
_mem(romPath), _romPath(romPath) ,_inputHandler(_mem)
{
	_sdlScreen.makeScreen();
	globalVars::running(true);
}

Gameboy::~Gameboy()
{

}

void Gameboy::run()
{
	double prev = 0;
	double current = 0;
	double deltaTime = 0;

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

			/****** actual input ******/
			_inputHandler.handleInputs(ev);
		}

		if (!LIMIT_SYS_FPS || deltaTime >= 1000.0 / SYS_FPS) //59.7 fps loop
		{
			//printf("%.2f\n", 1000.0 / deltaTime);

			_cpu.update();

			prev = current;
		}
	}

	SDL_DestroyWindow(_sdlScreen.getWindow());
	SDL_Quit();
}