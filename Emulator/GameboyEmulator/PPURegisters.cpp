#include "PPURegisters.h"

REG_LCDC::REG_LCDC(Memory& mem) : _val(mem[REG_LCDC_LOC]) 
{
	_val = 0x91;
}

bool REG_LCDC::getBit(const short index) const
{
	return (_val >> index) & 1;
}

void REG_LCDC::setBit(const short index, const bool val) const
{
	if (val)
	{
		_val |= (1 << index);
	}
	else
	{
		_val &= ~(1 << index);
	}
}

REG_STAT::REG_STAT(Memory& mem) : _val(mem[REG_STAT_LOC])
{
	_val = 0x81;
}

short REG_STAT::getMode() const
{
	return _val & 0b11;
}

void REG_STAT::setMode(const short& val)
{
	setBit(0, val & 1);
	setBit(1, (val & 10) >> 1);
}

bool REG_STAT::getLineComparator() const
{
	return _val & 0b100;
}

void REG_STAT::setLineComperator(const bool val)
{
	setBit(2, val);
}

bool REG_STAT::mode0IEget() const
{
	return _val & 0b1000;
}

void REG_STAT::mode0IEset(const bool val) const
{
	setBit(3, val);
}

bool REG_STAT::mode1IEget() const
{
	return _val & 0b10000;
}

void REG_STAT::mode1IEset(const bool val) const
{
	setBit(4, val);
}

bool REG_STAT::mode2IEget() const
{
	return _val & 0b100000;
}

void REG_STAT::mode2IEset(const bool val) const
{
	setBit(5, val);
}

bool REG_STAT::LYCIEget() const
{
	return _val & 0b1000000;
}

void REG_STAT::LYCIEset(const bool val) const
{
	setBit(6, val);
}


void REG_STAT::setBit(const short index, const bool val) const
{
	if (val)
	{
		_val |= (1 << index);
	}
	else
	{
		_val &= ~(1 << index);
	}
}