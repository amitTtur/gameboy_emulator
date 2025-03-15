#include "MBC2.h"

MBC2::MBC2(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag) 
	: MBC(memory, romPath,saveFolderPath,saveFlag)
{
	// checking limits
	if (_romView._romSize > KB256 || _romView._ramSize > 0) {
		throw GeneralException("IN MBC2 constractor the rom view didnt fit in the MBC limits...", MBC_UNDEFINED_BEHAVIOR);
	}

	// setting ram memory to 0
	for (int i = 0; i < 0x200; i++)
	{
		_mem[RAM_START + i] = 0xF0;
	}

	_ramg = false; // disabled by default
	_sram.resize(0); // no external ram in mbc2
}

MBC* MBC2::getMBC()
{
    return this;
}

void MBC2::updateBanks()
{
	int banks = _romView._romSize, romBank1; // currently holds the number of rom banks 2-512

	romBank1 = _bank1 & 0xF;

	if (romBank1 == 0x0 || romBank1 == 0x20 || romBank1 == 0x40 || romBank1 == 0x60) { romBank1++; }
	romBank1 &= (banks - 1);

	if (romBank1 != _activeRomBank)
	{
		loadBankToMem(romBank1, true);
	}
}

void MBC2::bankSwitchUpdate(const uint16_t& address, const uint8_t& value)
{
	if (address < BANK_SELECT_RANGE)
	{
		if ((address & 0x100) == 0) // if bit 8 is CLEAR (equal to 0) | ram enable/disable
		{
			_ramg = ((value & 0xF) == 0xA);
			//std::cout << "program has " << (_ramg ? "enabled" : "disabled") << " ram." << std::endl;
		}
		else // if bit 8 is SET | rom bank select
		{
			_bank1 = value;
			updateBanks();
		}
	}
}

void MBC2::writeToRam(const uint16_t& address, const uint8_t& value)
{
	if (_ramg )// if its a valid ram write
	{
		_mem[(RAM_START + (address & 0x1FF))] = (value & 0xF); // upper 4 bits are ignored in the write
	}
	else
	{
		std::cout << "rom attempeted to write to ram when the support is off... ignoring." << std::endl;
	}
}

uint8_t MBC2::readFromRam(const uint16_t& address) const
{
	if (_ramg) // if a valid read
	{
		// upper 4 bits are undefined
		return (_mem[(RAM_START + (address & 0x1FF))] & 0xF);
	}
	else
	{
		return 0xFF;
	}
}
