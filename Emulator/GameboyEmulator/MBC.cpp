#include "MBC.h"

MBC::MBC(uint8_t* memory, const std::string& romPath) : _mem(memory), _romView(getRomHeaders(romPath)), _activeRomBank(1)
{
	loadRomToCopy(romPath);
	//loadBiosToMem();
	
	// loading memory bank 0 from 0x100 (so bios wont be deleted) to 0x3FFF
	// usually rom dont have any data from 00 to 0xff cause the devs know it wouldnt be loaded
	loadBank0(); // there will always be a mem bank 0 so no need to check
	loadBankToMem(1);
	_activeRomBank = 1;
}

MBC::~MBC()
{
	// no need to delete the memory because iriginally it is a referance

}

void MBC::LoadRam(const uint8_t& ramBankNumber)
{
	// need to update current ram
	for (size_t i = 0; i < RAM_BANK_SIZE; i++)
	{
		// i offset + end of rom part in cartridge + start of current ram bank
		_romFileCopy[i + (MEMORY_BANK_SIZE * _romView._romSize) + (RAM_BANK_SIZE * _activeRamBank)] = _romFileCopy[i + (MEMORY_BANK_SIZE * _romView._romSize) + (RAM_BANK_SIZE * _activeRamBank)];
	}

	_activeRamBank = ramBankNumber;
	for (size_t i = 0; i < RAM_BANK_SIZE; i++)
	{
		// i offset + end of rom part in cartridge + start of current ram bank
		_mem[0xA000 + i] = _romFileCopy[i + (MEMORY_BANK_SIZE * _romView._romSize) + (RAM_BANK_SIZE * ramBankNumber)];
	}
}

void MBC::loadBankToMem(const uint8_t& bankNumber)
{
	// checking if bank is within the rom range
	if (bankNumber >= _romView._romSize)
	{
		throw GeneralException("Rom asked for a bank thats not in range WTF [MBC->loadBankToMem].", UNDEFINED_INSTRUCTION_BEHAVIOR);
	}

	size_t begin = bankNumber * MEMORY_BANK_SIZE;
	_activeRomBank = bankNumber;

	// loading
	for (size_t i = 0; i < MEMORY_BANK_SIZE; i++)
	{
		_mem[i + MEMORY_BANK_SIZE] = _romFileCopy[begin + i];
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

void MBC::loadBank0()
{
	// checking if bank is within the rom range
	if (_romView._romSize <= 0)
	{
		throw GeneralException("Rom asked for a bank thats not in range WTF [MBC->loadBankToMem].", UNDEFINED_INSTRUCTION_BEHAVIOR);
	}

	// range 0 to 0xff is ignored to not override the bios 
	// case of memory bank 0
	for (size_t i = 0; i < MEMORY_BANK_SIZE; i++)
	{
		_mem[i] = _romFileCopy[i];
	}
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

