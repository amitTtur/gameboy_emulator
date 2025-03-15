#include "MBC.h"

MBC::MBC(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag) : _mem(memory), _romView(getRomHeaders(romPath)), _activeRomBank(1)
{
	_isNoMbc = false;
	_saveFolderPath = saveFolderPath;
	_saveFlag = saveFlag;
	std::cout << "**************** MBC SETUP START ****************" << std::endl;
	loadRomToCopy(romPath);

	//loadBiosToMem();
	resizeSram(_romView._ramSize);

	//rom name extraction
	int firstIndex = romPath.find_last_of('\\');  // or '\\' for Windows paths
	if (firstIndex == std::string::npos) {
		firstIndex = romPath.find_last_of('/'); // Try alternate separator
	}
	int lastIndex = romPath.find_last_of('.');

	firstIndex = (firstIndex == std::string::npos) ? 0 : firstIndex + 1;
	lastIndex = (lastIndex == std::string::npos) ? romPath.size() : lastIndex;

	_gameName = romPath.substr(firstIndex, lastIndex - firstIndex);

	if (_romView._batterySupport && _saveFlag)
	{
		LoadTheSave();
	}

	// loading memory bank 0 from 0x100 (so bios wont be deleted) to 0x3FFF
	// usually rom dont have any data from 00 to 0xff cause the devs know it wouldnt be loaded
	loadBankToMem(0,false); // there will always be a mem bank 0 so no need to check
	loadBankToMem(1,true);
	_activeRomBank = 1;
	_activeRamBank = 0;
	_bank2 = 0;
	_bank1 = 0;
	_ramg = false;
	_mode = 0; // start on rom mode

	std::cout << "**************** MBC SETUP END ****************" << std::endl;
}

MBC::~MBC()
{
	// no need to delete the memory because originally it is a referance
	if (_romView._batterySupport && _saveFlag)
	{
		saveTheGame();
	}
}

uint8_t MBC::read(const uint16_t& address) const
{

	if (address == INPUT_REG_LOC)
	{
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

		


		uint8_t ret = _mem[INPUT_REG_LOC];
		//std::cout << int(ret) << std::endl; DEBUG
		uint8_t reversePad = ~(globalVars::padState());
		ret &= 0xf0;

		if ((ret & (1 << 4)) == 0)
		{
			ret |= ((reversePad >> 7) & 1) << 3; //down
			ret |= ((reversePad >> 6) & 1) << 2; //up
			ret |= ((reversePad >> 5) & 1) << 1; //left
			ret |= ((reversePad >> 4) & 1) << 0; //right
		}

		if ((ret & (1 << 5)) == 0)
		{
			ret |= ((reversePad >> 3) & 1) << 3; //start
			ret |= ((reversePad >> 2) & 1) << 2; //select
			ret |= ((reversePad >> 1) & 1) << 1; //b
			ret |= ((reversePad >> 0) & 1) << 0; //a
		}


		if (((ret >> 5) & 1) & ((ret >> 4) & 1))
		{
			ret |= 0x0f;
		}

		return ret;
	}

	// returned value when attempting to read from a rom with no mbc and no ram support is 0xFF
	if (address >= 0xA000 && address < 0xC000)
	{
		return readFromRam(address);
	}

	//vram
	if (address < 0xA000 && address >= 0x8000 && globalVars::vramAccess())
	{
		if (!globalVars::vramAccess())
		{
			// mode 3 
			std::cout << "rom attempeted to write to vram when the ppu is in a crutial mode... returning garbage." << std::endl;
			return 0xFF;
		}
		//oam
		if (address < 0xA000 && address >= 0x9800 && !globalVars::oamAccess())
		{
			// mode 3 and 2
			std::cout << "rom attempeted to read to vram (oam) when the ppu is in a crutial mode... returning garbage." << std::endl;
			return 0xFF;
		}
		// cpu allegible to write
		return this->_mem[address];
	}

	// for checks, remove after lcd is done
	// if (address == 0xFF44) { return 0x90; }

	// with no mbc the rom is treated as a large chunk of memory
	return this->_mem[address];
}

void MBC::write(const uint16_t& address, uint8_t value)
{
	// if a write to rom occurs
	// generally writes to those location are ignored cause you cant change hardwriten
	// game instruction of the rom, so the game boy uses those writes to change the behaviour
	// of the mbc. YOU clever clever boy

	// write special cases 
	if (checkTimerWrite(address, value))
	{
		return;
	}

	if (address < 0x8000)
	{
		bankSwitchUpdate(address, value);
		return;
	}

	//vram
	if (address < 0xA000 && address >= 0x8000 && globalVars::vramAccess())
	{
		//oam
		if (address < 0xA000 && address >= 0x9800 && !globalVars::oamAccess())
		{
			// mode 3 and 2
			std::cout << "rom attempeted to write to vram (oam) when the ppu is in a crutial mode... ignoring." << std::endl;
			return;
		}
		// cpu allegible to write
		_mem[address] = value;
		return;
	}

	// unchanable registers
	if (address == 0xFF44)
	{
		std::cout << "rom attempeted to write to a restricted register... ignoring." << std::endl;
		return;
	}
	if (address == 0xFF41)
	{
		_mem[address] = (_mem[address] & 0b10000000) | (0b01111000 & value) | (_mem[address] & 0b111);
		return;
	}

	if (address == INPUT_REG_LOC)
	{
		_mem[INPUT_REG_LOC] = (value & 0x30) | 0b11000000;
		return;
	}

	if (address == LYC_LOC)
	{
		_mem[address] = value;

		bool comp = _mem[LY_LOC] == _mem[LYC_LOC];
		_mem[REG_STAT_LOC] = (_mem[REG_STAT_LOC] & ~(1 << 2)) | (((comp) ? 1 : 0) << 2);
		if (((_mem[REG_STAT_LOC] >> 6) & 1) && ((_mem[REG_STAT_LOC] >> 2) & 1))
		{
			//_mem[INTERRUPT_FLAG_LOC] = (_mem[INTERRUPT_FLAG_LOC] & ~(1 << 2)) | (1 << 2);
			//_mem.int_LCDCStatus(1);
		}

		return;
	}

	if (address == REG_STAT_LOC)
	{
		_mem[address] = (value & 0b11111000) | (_mem[address] & 7); 
		return;
	}

	//dma
	if (address == DMA_LOC)
	{
		//std::cout << "dma should be called" << std::endl;
		globalVars::dma(true);
		_mem[address] = value; // dma function should be called
		return;
	}

	// attempt to write to ram when there is no ram support or when access to ram is disabled
	if (address >= 0xA000 && address < 0xC000)
	{
		writeToRam(address,value);
		return;
	}
	else
	{
		_mem[address] = value;
	}
}

uint16_t MBC::read16(const uint16_t& address) const
{
	return (read(address + 1) << 8) | read(address);
}


void MBC::writeToRam(const uint16_t& address, const uint8_t& value)
{
	if (!_ramg)
	{
		std::cout << "rom tried to write to ram when ram support is off... ignoring" << std::endl;
	}
	else
	{
		_mem[address] = value;
	}
}

uint8_t MBC::readFromRam(const uint16_t& address) const
{
	if (!_ramg)
	{
		return 0xFF;
	}
	else
	{
		return _mem[address];
	}
}

void MBC::resizeSram(RamSize size)
{
	// currently supports only mbc1 ram size
	_sram.resize(0x2000 * size,0x0); // ram bank size * number of banks
}

void MBC::LoadRam(const uint8_t& ramBankNumber,bool saveRamBank)
{
	if (saveRamBank)
	{
		//std::cout << "loading ram bank " << int(ramBankNumber) << " into memory." << std::endl;
		// need to update current ram
		for (size_t i = 0; i < RAM_BANK_SIZE; i++)
		{
			// i offset + end of rom part in cartridge + start of current ram bank
			_sram[i + (RAM_BANK_SIZE * _activeRamBank)] = _mem[RAM_AREA_START + i];
		}
	}

	_activeRamBank = ramBankNumber;
	for (size_t i = 0; i < RAM_BANK_SIZE; i++)
	{
		// i offset + end of rom part in cartridge + start of current ram bank
		_mem[0xA000 + i] = _sram[i + (RAM_BANK_SIZE * ramBankNumber)];
	}
}

void MBC::loadBankToMem(const uint8_t& bankNumber, bool range)
{
	// checking if bank is within the rom range
	if (bankNumber >= _romView._romSize)
	{
		throw GeneralException("Rom asked for a bank thats not in range WTF [MBC->loadBankToMem].", UNDEFINED_INSTRUCTION_BEHAVIOR);
	}
	int bnkcpy = int(bankNumber);
	//std::cout << "loading rom bank " << bnkcpy << " into memory." << std::endl;

	size_t begin = bankNumber * MEMORY_BANK_SIZE;

	if (range)
	{
		_activeRomBank = bankNumber;
	}
	else
	{
		_activeRomBank0 = bankNumber;
	}

	// loading
	for (size_t i = 0; i < MEMORY_BANK_SIZE; i++)
	{
		_mem[i + MEMORY_BANK_SIZE * range] = _romFileCopy[begin + i];
	}
}

bool MBC::checkTimerWrite(uint16_t address, uint8_t value)
{
	uint8_t divBit;
	switch (_mem[TIMER_TAC_LOC] & 0b11)
	{
	case 0b00: // bit 9
		divBit = (globalVars::DIVRegister() >> 9) & 1;
		break;
	case 0b01: // bit 3
		divBit = (globalVars::DIVRegister() >> 3) & 1;
		break;
	case 0b10: // bit 5
		divBit = (globalVars::DIVRegister() >> 5) & 1;
		break;
	case 0b11: // bit 7
		divBit = (globalVars::DIVRegister() >> 7) & 1;
		break;
	}

	if (address == TIMER_TIMA_LOC)
	{
		globalVars::TIMAOverflowCooldown(0);
		return true;
	}
	else if (address == TIMER_DIV_LOC)
	{

		if (divBit)
		{
			_mem[TIMER_TIMA_LOC] = _mem[TIMER_TIMA_LOC] + 1;
		}

		globalVars::DIVRegister(0);

		return true;
	}
	else if (address == TIMER_TAC_LOC)
	{

		_mem[TIMER_TAC_LOC] = value;

		if (value >> 2 == 0 && divBit)
		{
			_mem[TIMER_TIMA_LOC] = _mem[TIMER_TIMA_LOC] + 1;
		}

		uint8_t newDivBit;
		switch (_mem[TIMER_TAC_LOC] & 0b11)
		{
		case 0b00: // bit 9
			newDivBit = (globalVars::DIVRegister() >> 9) & 1;
			break;
		case 0b01: // bit 3
			newDivBit = (globalVars::DIVRegister() >> 3) & 1;
			break;
		case 0b10: // bit 5
			newDivBit = (globalVars::DIVRegister() >> 5) & 1;
			break;
		case 0b11: // bit 7
			newDivBit = (globalVars::DIVRegister() >> 7) & 1;
			break;
		}

		if (newDivBit && !divBit)
		{
			_mem[TIMER_TIMA_LOC] = _mem[TIMER_TIMA_LOC] + 1;
		}

		return true;
	}
	return false;
}


uint8_t* MBC::getRomHeaders(const std::string& romPath)
{
	// is deleted in cartridge view constructor
	uint8_t* tempArr = new uint8_t[0x150];

	std::fstream romFile;

	// Opening ROM file in read-only binary mode
	romFile.open(romPath, std::ios::in | std::ios::binary);

	if (!romFile.is_open()) // Check if file opening failed
	{
		delete[] tempArr;
		throw GeneralException("Tried to open ROM file in [MBC->getRomHeaders], but failed to open...", FILE_WONT_OPEN);
	}

	// file size
	romFile.seekg(0, std::ios::end);
	std::streamsize size = romFile.tellg();

	if (size <= 0) // bad rom
	{
		delete[] tempArr;
		throw GeneralException("The ROM file appears to be empty or unreadable. ERROR_HANDELING_ROM", ERROR_HANDELING_ROM);
	}

	romFile.seekg(0, std::ios::beg); // Reset the file pointer to the beginning

	// Copying file content to a local variable
	romFile.read(reinterpret_cast<char*>(tempArr), 0x150);

	// Check for read errors
	if (!romFile) {
		delete[] tempArr;
		throw GeneralException("Failed to read the ROM file completely. ERROR_HANDELING_ROM", ERROR_HANDELING_ROM);
	}

	romFile.close();
	return tempArr;
}

void MBC::write16(const uint16_t& address, uint16_t value)
{
	write(address, value & 0xFF);
	write(address, (value >> 8) & 0xFF);
}

void MBC::createRomFileCopy(const std::string& folderPath, const std::string& copyName)
{
	std::filesystem::path downloadPath;
	// means the user want the rom in downloads
	if (!folderPath.empty())
	{
		#pragma warning(suppress : 4996);
		const char* userProfile = getenv("USERPROFILE");
		if (!userProfile) {
			std::cout << "user is in a non-windows environement, copying file to the [c] disk." << std::endl;
		}
		else // he is in a windows env
		{
			std::filesystem::path downloadPath = std::filesystem::path(userProfile) / "Downloads";
			
		}
	}
	// means a good folder has been given
	else if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath))
	{
		downloadPath = folderPath;
	}
}

void MBC::loadRomToCopy(const std::string& romPath)
{
	std::fstream romFile;

	// Opening ROM file in read-only binary mode
	romFile.open(romPath, std::ios::in | std::ios::binary);

	if (!romFile.is_open()) // Check if file opening failed
	{
		throw GeneralException("Tried to open ROM file in [MBC->loadRomToCopy], but failed to open...", FILE_WONT_OPEN);
	}

	std::cout << "ROM file opened successfully!" << std::endl;

	// file size
	romFile.seekg(0, std::ios::end);
	std::streamsize size = romFile.tellg();

	if (size <= 0) // bad rom
	{ 
		throw GeneralException("The ROM file appears to be empty or unreadable. ERROR_HANDELING_ROM", ERROR_HANDELING_ROM);
	}

	romFile.seekg(0, std::ios::beg); // Reset the file pointer to the beginning

	// Copying file content to a local variable
	_romFileCopy.resize(static_cast<size_t>(size)); // Resize vector 
	romFile.read(_romFileCopy.data(), size);

	// Check for read errors
	if (!romFile) {
		throw GeneralException("Failed to read the ROM file completely. ERROR_HANDELING_ROM", ERROR_HANDELING_ROM);
	}

	romFile.close();
	std::cout << "ROM file closed successfully!" << std::endl;
}

void MBC::saveTheGame()
{
	if (!_sram.size()) { return; }


	// need to update current ram
	for (size_t i = 0; i < RAM_BANK_SIZE; i++)
	{
		// i offset + end of rom part in cartridge + start of current ram bank
		_sram[i + (RAM_BANK_SIZE * _activeRamBank)] = _mem[RAM_AREA_START + i];
	}

	// 
	std::string saveFilePath = _saveFolderPath + _gameName + ".save";
	
	std::ofstream saveFile;
	saveFile.open(saveFilePath, std::ios::binary);

	if (!saveFile) {
		std::cout << "Failed to open save file for writing at: " << saveFilePath << std::endl;
		return;
	}

	saveFile.write(reinterpret_cast<const char*>(_sram.data()), _sram.size());

	if (!saveFile.good()) {
		std::cerr << "Error writing save file!" << std::endl;
	}
	else {
		std::cout << "Game saved successfully to " << saveFilePath << std::endl;
	}

	saveFile.close();
}

void MBC::LoadTheSave()
{
	if (!_sram.size()) { return; }

	std::string savePath = _saveFolderPath + _gameName + ".save";
	std::ifstream saveFile(savePath, std::ios::binary);

	if (!saveFile) {
		std::cerr << "Failed to open save file for reading at: " << savePath << "\n";
		return;
	}

	saveFile.seekg(0,std::ios::end);
	int size = saveFile.tellg();
	saveFile.seekg(0, std::ios::beg);
	if(_sram.size() != size) 
	{
		saveFile.close();
		std::cout << "save size doesnt match the saevvs of the game..." << std::endl;
		return;
	}

	saveFile.read(reinterpret_cast<char*>(_sram.data()), _sram.size());

	if (!saveFile.good()) {
		std::cout << "Error reading save file!" << std::endl;
		saveFile.close();
		return;
	}

	if (_romView._ramSize) {
		_activeRamBank = 0;
		LoadRam(0, false);  // false means don't save current bank first
	}

	std::cout << "Save file loaded successfully from " << savePath << std::endl;
	saveFile.close();
}


void MBC::loadBiosToMem()
{
	std::fstream romFile;

	// Opening ROM file in read-only binary mode
	romFile.open(BOOT_LOC, std::ios::in | std::ios::binary);

	if (!romFile.is_open()) // Check if file opening failed
	{
		throw GeneralException("Tried to open boot file in [MBC->loadBiosToMem], but failed to open...", FILE_WONT_OPEN);
	}

	std::cout << "boot file opened successfully!" << std::endl;

	// file size
	romFile.seekg(0, std::ios::end);
	std::streamsize size = romFile.tellg();

	if (size <= 0 || size != 0x100) // bad rom
	{
		throw GeneralException("The ROM file appears to be empty or unreadable or isnt 0x100 in size. ERROR_HANDELING_ROM", ERROR_HANDELING_ROM);
	}

	romFile.seekg(0, std::ios::beg); // Reset the file pointer to the beginning

	// Copying file content to a local variable
	std::vector<char> tempBoot;
	tempBoot.resize(static_cast<size_t>(size)); // Resize vector 
	romFile.read(tempBoot.data(), size);

	// Check for read errors
	if (!romFile) {
		throw GeneralException("Failed to read the ROM file completely. ERROR_HANDELING_ROM", ERROR_HANDELING_ROM);
	}

	for (size_t i = 0; i < size; i++)
	{
		_romFileCopy[i] = tempBoot[i];
	}

	romFile.close();
	std::cout << "boot file closed successfully!" << std::endl;
}

