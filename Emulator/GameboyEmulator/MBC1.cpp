#include "MBC1.h"

MBC1::MBC1(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag) : 
	MBC(memory,romPath,saveFolderPath,saveFlag)
{
	// checking limits
	if (_romView._romSize > 128 || _romView._ramSize > 8) {
		throw GeneralException("IN MBC1 constractor the rom view didnt fit in the MBC limits...", MBC_UNDEFINED_BEHAVIOR);
	}
}

MBC* MBC1::getMBC()
{
	return this;
}

void MBC1::updateBanks()
{
	int banks = _romView._romSize, romBank1, ramBank, romBank0; // currently holds the number of rom banks 2-512

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
	ramBank &= (_romView._ramSize - 1);

	if (ramBank != _activeRamBank && ramBank < _romView._ramSize)
	{
		LoadRam(ramBank, true);
	}
	if (romBank1 != _activeRomBank)
	{
		loadBankToMem(romBank1, true);
	}
	if (romBank0 != _activeRomBank0)
	{
		loadBankToMem(romBank0, false);
	}
}

void MBC1::bankSwitchUpdate(const uint16_t& address, const uint8_t& value)
{
	if (address <= RAM_ENABLE_HIGH && address >= RAM_ENABLE_LOW) // ram support toggle
	{
		_ramg = ((value & 0xF) == 0xA); // if ramg == 0xA0
		//std::cout << "program has " << (_ramg ? "enabled" : "disabled") << " ram." << std::endl;
	}
	else
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
			//std::cout << "program set the mode to " << (_mode ? "ram" : "rom") << " managment." << std::endl;
		}
		updateBanks();
	}
}
