#include "MBC1.h"

MBC1::MBC1(uint8_t* memory, const std::string& romPath) : MBC(memory,romPath)
{
	// checking limits
	if (_romView._romSize > 128 || _romView._ramSize > 8) {
		throw GeneralException("IN MBC1 constractor the rom view didnt fit in the MBC limits...", MBC_UNDEFINED_BEHAVIOR);
	}

	switch (_romView._ramSize)
	{
	case 1:
	{
		_ramMask = 0;
		break;
	}
	case 4:
	{
		_ramMask = 3;
	}
	default:
	{
		_ramMask = 0;
	}
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
		LoadRam(ramBank);
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