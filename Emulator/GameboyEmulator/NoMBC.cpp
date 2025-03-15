#include "NoMBC.h"

NoMBC::NoMBC(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag) :
	MBC(memory,romPath,saveFolderPath,saveFlag)
{
	_isNoMbc = true;
	// checking limits
	if (_romView._romSize > KB32 || _romView._ramSize > ramKB8) {
		throw GeneralException("IN MBC1 constractor the rom view didnt fit in the MBC limits...", MBC_UNDEFINED_BEHAVIOR);
	}
}

MBC* NoMBC::getMBC()
{
	return this;
}

void NoMBC::updateBanks()
{
	// no bank switching logic is needed in no mbc
}

void NoMBC::bankSwitchUpdate(const uint16_t& address, const uint8_t& value)
{
	if (address <= RAM_ENABLE_HIGH && address >= RAM_ENABLE_LOW) // ram support toggle
	{
		_ramg = ((value & 0xF) == 0xA); // if ramg == 0xA0
	}
}
