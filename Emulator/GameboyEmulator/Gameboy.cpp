#include "Gameboy.h"

Gameboy::Gameboy() : _cpu(_ppu, _mem), _ppu(_mem, _sdlScreen),_inputHandler(_mem) {}

Gameboy::~Gameboy() {}

void Gameboy::init()
{
	bool debug;
	bool colorPalette;
	int screenSize;
	bool saveProgress;
	std::string saveFolderPath;
	if (!readConfigFile(CONFIG_FILE_NAME, debug, screenSize, colorPalette,saveProgress,saveFolderPath))
	{
		std::cout << "error reading config file" << std::endl;
		return;
	}

	_sdlScreen.makeScreen(screenSize, colorPalette);
	bool waitingForRom = true;

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	while (waitingForRom)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			if (SDL_QUIT == ev.type)
			{
				waitingForRom = false;
				break;
			}

			if (ev.type == SDL_DROPFILE)
			{
				_romPath = ev.drop.file;
				std::cout << "file dropped " << _romPath << std::endl;
				waitingForRom = false;
				break;
			}
		}
	}

	try
	{
		_mem.init(_romPath,saveFolderPath,saveProgress);
		globalVars::running(true);
		run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
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

		if (!LIMIT_SYS_FPS || deltaTime >= 1000.0 / SYS_FPS) //59.7 fps loop
		{
			//printf("%.2f\n", 1000.0 / deltaTime);

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

			_cpu.update();

			prev = current;
		}
	}

	SDL_DestroyWindow(_sdlScreen.getWindow());
	SDL_Quit();
}

bool Gameboy::readConfigFile(const std::string& filename, bool& debugMode, int& screenSize, bool& colorPalette, bool& saveProgress, std::string& saveFolderPath)
{
	std::ifstream configFile(filename);

	if (!configFile.is_open()) {
		globalVars::setColor(4);
		std::cout << "Error: Could not open the configuration file!" << std::endl;
		globalVars::setColor(7);
		return false;
	}

	std::string line;
	bool saveFlag = false; // for edge case in the saev proggress and invalid pathway
	while (getline(configFile, line)) {
		std::stringstream ss(line);
		std::string key;
		std::string value;

		if (getline(ss, key, '=') && getline(ss, value)) {

			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			if (key == "debugMode") {
				debugMode = (value == "true");
			}
			else if (key == "screenSize") {
				std::stringstream(value) >> screenSize;
			}
			else if (key == "colorPalette") {
				colorPalette = (value == "true");
			}
			else if (key == "saveProgress")
			{
				
				saveProgress = (!saveFlag) && (value == "true" || value == "True" || value == "TRUE" || value == "1"); // please use this feature, im begging you
			}
			else if (key == "saveFolderPath")
			{
				// remove brackets if any
				if (value[0] == '"' || value[0] == '\'')
				{
					value = value.substr(1);
				}
				if (value[value.size() - 1] == '"' || value[value.size() - 1] == '\'')
				{
					value.pop_back();
				}
				for (char c : value)
				{
					saveFolderPath += (c == '\\') ? '/' : c; // the '\\' char could have diffrent meaning based on the next character, i flip it to avoid errors/
				}
				// if the folder exists
				if (!std::filesystem::is_directory(saveFolderPath))
				{
					globalVars::setColor(4);
					std::cout << "Error: given save folder pathway in the config file doesnt exist in your computer..." << std::endl <<
						"save would not be loaded and progress wouldnt be saved." << std::endl;
					globalVars::setColor(7);
					saveFlag = true;
					saveProgress = false;
				}
			}
		}
	}

	configFile.close();
	std::cout << "config file" << std::endl;
	return true;
	return false;
}