#include "NoMBC.h"

NoMBC::NoMBC(uint8_t* memory, const std::string& romPath) : MBC(memory,romPath)
{
	_isNoMbc = true;
	resizeSram(_romView._ramSize);
	if (_romView._ramSupport && _romView._ramSize > 0)
	{
		LoadRam(0); // nombc support up to 1 ram bank so only bank 0 exist if any
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