#include "MBC1.h"

MBC1::MBC1(uint8_t* memory, const std::string& romPath) : MBC(memory,romPath) , _mode(0)
{
	// checking limits
	if (_romView._romSize > 128 || _romView._ramSize > 4) {
		throw GeneralException("IN MBC1 constractor the rom view didnt fit in the MBC limits...", MBC_UNDEFINED_BEHAVIOR);
	}

	// putting bank1 mask
	switch (_romView._romSize)
	{
	case 2:
	{
		_bank1Mask = 0x1;
		break;
	}
	case 4:
	{
		_bank1Mask = 0x3;
		break;
	}
	case 8:
	{
		_bank1Mask = 0x7;
		break;
	}
	case 16:
	{
		_bank1Mask = 0xF;
		break;
	}
	case 32:
	{
		_bank1Mask = 0x1F;
		break;
	}
	}

}

MBC* MBC1::getMBC()
{
	return this;
}

uint8_t MBC1::read(const uint16_t& address) const
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

void MBC1::write(const uint16_t& address, uint8_t value)
{
	// if a write to rom occurs
	// generally writes to those location are ignored cause you cant change hardwriten
	// game instruction of the rom, so the game boy uses those writes to change the behaviour
	// of the mbc. YOU clever clever boy

	// write special cases 
	if (address == TIMER_DIV_LOC)
	{
		globalVars::DIVRegister(0);
		return;
	}

	if (address < 0x8000)
	{
		// activating ram support
		// if written to 0x0 to 0x1FFF and the value in lower 4 bits is 0xA any other value disables it
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
		else if (address <= 0x3FFF || address >= 0x2000)
		{
			setBank1(value);
		}
		else if (address <= 0x5FFF || address >= 0x4000)
		{
			setSecondaryBank(value);
			if (_mode)
			{
				loadBankToMem(giveCurrentActiveRomBank());
			}
			else
			{
				LoadRam(giveCurrentActiveRamBank());
			}
		}
		else // 0x6000 - 0x7FFF mode set
		{
			_mode = value & 1; // lsb decides if its on or off
			if (_mode)
			{
				LoadRam(0); // mode 1 is locked to ram bank 0
			}
			// if mode is set to mode 1 no changes are needed
		}
		return;
	}

	if (address == TIMER_TIMA_LOC)
	{
		globalVars::TIMAOverflowCooldown(0);
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

uint16_t MBC1::read16(const uint16_t& address) const
{
	return (read(address+1) << 8) | read(address); 
}

void MBC1::write16(const uint16_t& address, uint16_t value)
{
	_mem[address] = value & 0xFF;           
	_mem[address + 1] = (value >> 8) & 0xFF; 
}

void MBC1::setBank1(const uint8_t& val)
{
	uint8_t tmp = val & 0xb00011111;
	tmp = tmp == 0 ? 1 : tmp; // it is a known bug that the gb does that allws for duplication of bank 1 in mem
	_bank1 = tmp & _bank1Mask;
}

void MBC1::setSecondaryBank(const uint8_t& val)
{
	_secondaryBank = val & _secondaryBankMask;
}

uint8_t MBC1::giveCurrentActiveRamBank() const
{
	return _secondaryBank * _mode;
}

uint8_t MBC1::giveCurrentActiveRomBank() const
{
	return (_secondaryBank << 5 ) * _mode + _bank1;
}
