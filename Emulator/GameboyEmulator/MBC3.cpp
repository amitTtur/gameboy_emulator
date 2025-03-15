#include "MBC3.h"

MBC3::MBC3(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag)
	: MBC(memory, romPath,saveFolderPath,saveFlag)
{
	// checking limits
	if (_romView._romSize > MB2 || _romView._ramSize > ramKB32) {
		throw GeneralException("IN MBC3 constractor the rom view didnt fit in the MBC limits...", MBC_UNDEFINED_BEHAVIOR);
	}

	// clock start ticking
	_clockHalt = false; // on default it is ticking
	_latchSequence = -1;
}

MBC* MBC3::getMBC()
{
	return this;
}

void MBC3::updateBanks()
{
	int banks = _romView._romSize, romBank1 = _bank1, ramBank = _bank2; // currently holds the number of rom banks 2-512

	//if (romBank1 == 0x0 || romBank1 == 0x20 || romBank1 == 0x40 || romBank1 == 0x60) { romBank1++; }
	romBank1 &= (banks - 1);

	if (!(ramBank == 0x8 || ramBank == 0x9 || ramBank == 0xA || ramBank == 0xB || ramBank == 0xC)) // thise addresses should not
	{                                                                                              // be called
		ramBank &= (_romView._ramSize - 1);
		if (ramBank != _activeRamBank && ramBank < _romView._ramSize)
		{
			LoadRam(ramBank, true);
		}
	}
	if (romBank1 != _activeRomBank)
	{
		loadBankToMem(romBank1, true);
	}
}

void MBC3::bankSwitchUpdate(const uint16_t& address, const uint8_t& value)
{
	if (address <= RAM_ENABLE_HIGH && address >= RAM_ENABLE_LOW) // ram support toggle
	{
		_ramg = ((value & 0xF) == 0xA); // if ramg == 0xA0
		//std::cout << "program has " << (_ramg ? "enabled" : "disabled") << " ram." << std::endl;
	}
	else
	{
		if (address >= ROM_LOW && address <= ROM_HIGH) // rom bank select
		{
			_bank1 = value;
			//std::cout << "program has set the value of bank1 to " << int(_bank1) << std::endl;
		}
		else if (address <= RAM_RTC_LOW && address >= RAM_RTC_HIGH)
		{
			_bank2 = value;
		}
		else if (address <= LATCH_LOW && address >= LATCH_HIGH) // ram bank select
		{
			if (_latchSequence == -1 && value == 0) { _latchSequence++; }
			else if (_latchSequence == 0 && value == 1) { updateRTCRegs(); _latchSequence = -1; }
			else { _latchSequence = -1; }
		}
		updateBanks();
	}
}

void MBC3::updateRTCRegs()
{
	int timeToAdd = globalVars::clock, tmp = 0;
	globalVars::clock = 0;

	// Update seconds
	tmp = (timeToAdd + _rtcSeconds) / 60;
	_rtcSeconds = (timeToAdd + _rtcSeconds) % 60;

	// Update minutes
	tmp += _rtcMinutes;
	_rtcMinutes = tmp % 60;
	tmp /= 60;

	// Update hours
	tmp += _rtcHours;
	_rtcHours = tmp % 24;
	tmp /= 24;

	// Update days - handle potential overflow
	int newDays = tmp + (_rtcDaysLv | (_rtcDaysHv << 8));

	// Check if we exceed 511 days (9-bit counter)
	if (newDays > 511) {
		_daysCarry = true;
		newDays %= 512;
	}

	// Update days registers
	_rtcDaysLv = newDays & 0xFF;        // Lower 8 bits
	_rtcDaysHv = (newDays >> 8) & 0x01;  // Upper 1 bit
}


void MBC3::writeToRam(const uint16_t& address, const uint8_t& value)
{
	if (!_ramg) { return; }

	switch (_bank2)
	{
	case 0x8:
	{
		_rtcSeconds = value;
		break;
	}
	case 0x9:
	{
		_rtcMinutes = value;
		break;
	}
	case 0xA:
	{
		_rtcHours = value;
		break;
	}
	case 0xB:
	{
		_rtcDaysLv = value;
		break;
	}
	case 0xC:
	{
		_generalRtcReg = value;
		globalVars::canIncreaseMbc3Timer(_clockHalt);
		break;
	}
	default:
	{
		_mem[address] = value;
		break;
	}
	}
}

uint8_t MBC3::readFromRam(const uint16_t& address) const
{
	if (!_ramg) { return 0xFF; }

	switch (_bank2)
	{
	case 0x8:
	{
		return _rtcSeconds;
	}
	case 0x9:
	{
		return _rtcMinutes;
	}
	case 0xA:
	{
		return _rtcHours;
	}
	case 0xB:
	{
		return _rtcDaysLv;
	}
	case 0xC:
	{
		return _generalRtcReg;
	}
	default:
	{
		return _mem[address];
	}
	}
}
