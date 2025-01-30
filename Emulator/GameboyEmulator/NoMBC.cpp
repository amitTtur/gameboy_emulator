#include "NoMBC.h"

NoMBC::NoMBC(uint8_t* memory, const std::string& romPath) : MBC(memory,romPath)
{
	if (_romView._ramSupport && _romView._ramSize > 0)
	{
		LoadRam(0); // nombc support up to 1 ram bank so only bank 0 exist if any
	}
}

MBC* NoMBC::getMBC()
{
	return this;
}

uint8_t NoMBC::read(const uint16_t& address) const
{
	// returned value when attempting to read from a rom with no mbc and no ram support is 0xFF
	if (!this->_romView._ramSupport && address >= 0xA000 && address < 0xC000)
	{
		return 0xFF;
	}

	// for checks, remove after lcd is done
	if (address == 0xFF44) { return 0x90; }

	// with no mbc the rom is treated as a large chunk of memory
	return this->_mem[address];
}

void NoMBC::write(const uint16_t& address, uint8_t value)
{
	// activating ram support
	// if written to 0x0 to 0x1FFF and the value in lower 4 bits is 0xA any other value disables it
	if (address < 0x8000)
	{
		if (address <= 0x1FFF)
		{
			_ramg = value & 0xF;
			if (_ramg == 0xA) // enables
			{
				if (!_romView._ramSize)
				{
					throw GeneralException("instruction activated ram support when there is no ram in rom file", MBC_UNDEFINED_BEHAVIOR);
				}
			}
		}
		else
		{
			// cant write to memory except ram when there is no mbc
			std::cout << "rom attempeted to write to rom when there is no mbc... ignoring." << std::endl;
		}
	}

	// write special cases 
	if (address == TIMER_TIMA_LOC)
	{
		globalVars::TIMAOverflowCooldown(0);
	}
	else if (address == TIMER_DIV_LOC)
	{
		globalVars::DIVRegister(0);
	}



	// attempt to write to ram when there is no ram support or when access to ram is disabled
	if (address >= 0xA000 && address < 0xC000 && (!_romView._ramSupport || _ramg != 0xA))
	{
		std::cout << "rom attempeted to write to ram when the support is off... ignoring." << std::endl;
		return;
	}
	else
	{
		_mem[address] = value;
	}
}

uint16_t NoMBC::read16(const uint16_t& address) const
{
	return (read(address + 1) << 8) | read(address);
}

void NoMBC::write16(const uint16_t& address, uint16_t value)
{
	_mem[address] = value & 0xFF;
	_mem[address + 1] = (value >> 8) & 0xFF;
}
