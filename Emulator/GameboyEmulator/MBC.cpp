#include "MBC.h"

MBC::MBC(uint8_t* memory, const std::string& romPath) : _mem(memory), _romView(getRomHeaders(romPath)), _activeRomBank(1)
{
	_isNoMbc = false;
	std::cout << "**************** MBC SETUP START ****************" << std::endl;
	loadRomToCopy(romPath);
	//loadBiosToMem();
	resizeSram(_romView._ramSize);
	// loading memory bank 0 from 0x100 (so bios wont be deleted) to 0x3FFF
	// usually rom dont have any data from 00 to 0xff cause the devs know it wouldnt be loaded
	loadBankToMem(0,false); // there will always be a mem bank 0 so no need to check
	loadBankToMem(1,true);
	_activeRomBank = 1;
	_activeRamBank = 0;
	_bank2 = 0;
	_bank1 = 0;
	_ramg = false;
	if (_ramg && 0 < _romView._ramSize)
	{
		LoadRam(0);
	}
	_mode = 0; // start on rom mode

	std::cout << "**************** MBC SETUP END ****************" << std::endl;
}

MBC::~MBC()
{
	// no need to delete the memory because originally it is a referance

}

uint8_t MBC::read(const uint16_t& address) const
{


	// returned value when attempting to read from a rom with no mbc and no ram support is 0xFF
	if (!_ramg && address >= 0xA000 && address < 0xC000)
	{
		return 0xFF;
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
		bankSwitchUpdate(_isNoMbc, address, value);
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
		_mem[INPUT_REG_LOC] = (_mem[INPUT_REG_LOC] & 0xf) | (value & 0x30);
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
	if (address >= 0xA000 && address < 0xC000 && !_ramg)
	{
		std::cout << "rom attempeted to write to ram when the support is off... ignoring." << std::endl;
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

void MBC::resizeSram(RamSize size)
{
	// currently supports only mbc1 ram size
	_sram.resize(0x2000 * size,0x0); // ram bank size * number of banks
}

void MBC::LoadRam(const uint8_t& ramBankNumber)
{
	std::cout << "loading ram bank " << int(ramBankNumber) << " into memory." << std::endl;
	// need to update current ram
	for (size_t i = 0; i < RAM_BANK_SIZE; i++)
	{
		// i offset + end of rom part in cartridge + start of current ram bank
		_sram[i + (RAM_BANK_SIZE * _activeRamBank)] = _mem[RAM_AREA_START + i];
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
	std::cout << "loading rom bank " << bnkcpy << " into memory." << std::endl;

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

void MBC::bankSwitchUpdate(bool noMbc, const uint16_t& address, const uint8_t& value)
{
	if (address <= RAM_ENABLE_HIGH && address >= RAM_ENABLE_LOW) // ram support toggle
	{
		_ramg = ((value & 0xF) == 0xA); // if ramg == 0xA0
		std::cout << "program has " << (_ramg ? "enabled" : "disabled") << " ram." << std::endl;
	}
	else if (!noMbc)
	{
		if (address <= ROM_BANK_SET_HIGH && address >= ROM_BANK_SET_LOW) // rom bank select
		{
			_bank1 = value;
			//std::cout << "program has set the value of bank1 to " << int(_bank1) << std::endl;
		}
		else if (address <= RAM_BANK_HIGH && address >= RAM_BANK_LOW) // ram bank select
		{
			_bank2 = value;
			
			//std::cout << "program has set the value of bank2 to " << int(_bank2) << std::endl;
		}
		else // mode select
		{
			_mode = value;
			std::cout << "program set the mode to " << (_mode ? "ram" : "rom") << " managment." << std::endl;
		}
		updateBanks();
	}
}

void MBC::updateBanks()
{
	int banks = _romView._romSize,romBank1,ramBank,romBank0; // currently holds the number of rom banks 2-512

	if (banks >= 64) // if bank2 is needed in the rom bank calc, if is true then mode = 0 (rom mode)
	{
		romBank0 = (_mode & 1) ? (_bank2 & 3) << 5 : 0;
		ramBank = 0;
		romBank1 = ((_bank2 & 3) << 5) | (_bank1 & 0x1F);
	}
	else
	{
		ramBank = (_mode & 1) ? (_bank2) & 3 : 0;
		romBank0 = 0;
		romBank1 = _bank1 & 0x1F;
	}
	if (romBank1 == 0x0 || romBank1 == 0x20 || romBank1 == 0x40 || romBank1 == 0x60) { romBank1++; }
	romBank1 &= (banks - 1);
	romBank0 &= (banks - 1);
	ramBank &= (_romView._ramSize-1);

	if (ramBank != _activeRamBank && ramBank < _romView._ramSize)
	{
		LoadRam(ramBank);
	}
	if (romBank1 != _activeRomBank)
	{
		loadBankToMem(romBank1,true);
	}
	if (romBank0 != _activeRomBank0)
	{
		loadBankToMem(romBank0, false);
	}
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

