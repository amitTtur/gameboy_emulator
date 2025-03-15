#include "MBC5.h"

MBC5::MBC5(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag) :
	MBC(memory, romPath,saveFolderPath,saveFlag)
{
	// checking limits
	if (_romView._romSize > MB8 || _romView._ramSize > ramKB128) {
		throw GeneralException("IN MBC5 constractor the rom view didnt fit in the MBC limits...", MBC_UNDEFINED_BEHAVIOR);
	}
}

MBC* MBC5::getMBC()
{
    return this;
}

void MBC5::updateBanks()
{
	int banks = _romView._romSize, romBank1, ramBank; // currently holds the number of rom banks 2-512

	romBank1 = ((_romBankMsb & 1) << 8) | _bank1;
	ramBank = _bank2;

	// if bit 3 of ram bank is on, rumble is active
	globalVars::rumble((ramBank & 0b1000) != 0);

	//if (romBank1 == 0x0 || romBank1 == 0x20 || romBank1 == 0x40 || romBank1 == 0x60) { romBank1++; }
	romBank1 &= (banks - 1);
	ramBank &= (_romView._ramSize - 1);

	if (ramBank != _activeRamBank && ramBank < _romView._ramSize)
	{
		LoadRam(ramBank,true);
	}
	if (romBank1 != _activeRomBank)
	{
		loadBankToMem(romBank1, true);
	}
}

void MBC5::bankSwitchUpdate(const uint16_t& address, const uint8_t& value)
{
	if (address >= 0x6000) { return; }

	if (address <= RAM_ENABLE_HIGH && address >= RAM_ENABLE_LOW) // ram support toggle
	{
		_ramg = ((value & 0xF) == 0xA); // if ramg == 0xA0
		//std::cout << "program has " << (_ramg ? "enabled" : "disabled") << " ram." << std::endl;
	}
	else
	{
		if (address <= ROM_BANK_1_HIGH && address >= ROM_BANK_1_LOW) // rom bank select
		{
			_bank1 = value;
			//std::cout << "program has set the value of bank1 to " << int(_bank1) << std::endl;
		}
		else if (address <= ROM_BANK_2_HIGH && address >= ROM_BANK_2_LOW)
		{
			_romBankMsb = value;
		}
		else if (address <= RAM_BANK_HIGH && address >= RAM_BANK_LOW) // ram bank select
		{
			_bank2 = value;

			//std::cout << "program has set the value of bank2 to " << int(_bank2) << std::endl;
		}
		updateBanks();
	}
}
