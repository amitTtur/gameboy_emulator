#include "Memory.h"

Memory::Memory(const std::string& romPath) : 
	_IE("IE", _mem[INTERRUPT_ENABLE_LOC]), _IF("IF", _mem[INTERRUPT_FLAG_LOC]) 
{
	_IME = 1;

	switch (cartridgeView::getType(MBC::getRomHeaders(romPath)))
	{
	case mbc1:
	{
		_mbc = new MBC1(_mem, romPath);
		break;
	}
	case romOnly:
	{
		_mbc = new NoMBC(_mem, romPath);
		break;
	}
	default:
	{
		throw GeneralException("The selected mbc type isn't supported.", MBC_NOT_SUPPORTED);
	}
	}
}

Memory::~Memory()
{
	delete _mbc;
}

uint8_t& Memory::operator[](const uint16_t& index)
{
	return this->_mem[index];
}

RegisterFile& Memory::getsRegs()
{
	return this->_regs;
}

uint8_t Memory::getMemoryLocation()
{
	uint8_t temp = _mbc->read(_regs.PC);
	_regs.PC += 1;
	return temp;
}

uint16_t Memory::getMemoryLocation16bit() {
	uint8_t lowByte = _mbc->read(_regs.PC);        // Read the high byte
	uint8_t highByte = _mbc->read(_regs.PC + 1);     // Read the low byte

	// Combine the two bytes into a 16-bit value (little-endian)
	uint16_t tmp = (highByte << 8) | lowByte;

	_regs.PC += 2;

	// Return the 16-bit value
	return tmp;
}


uint8_t Memory::getMemoryLocation(const uint16_t& index)
{
	return _mbc->read(index);
}

uint16_t Memory::getMemoryLocation16bit(const uint16_t& index) {
	// Use MBC read to get the first (low byte) and second (high byte) bytes
	uint8_t lowByte = _mbc->read(index);         // Read low byte
	uint8_t highByte = _mbc->read(index + 1);    // Read high byte

	// Combine the two bytes into a 16-bit value in little-endian order
	return static_cast<uint16_t>(lowByte) | (static_cast<uint16_t>(highByte) << 8);
}


operandReturn<uint8_t> Memory::get8BitOperand(const operandElementHolder& operand) {
	specialLdMove = false;
	operandReturn<uint8_t> ret;
	if (operand.immediate) // VERY VERY VERY IMPORTANT CHECK FOR THE 16 bit ones!!!!
	{
		if (operand.name == "n8" || operand.name == "e8" || operand.name == "a8")
		{
			ret.value = getMemoryLocation();
			return ret; // not directily bacause that function should increase the
		}                               // cycles, (cause fetching a memory location takes 1 cycle).

		ret.isRegister = operand.name;
		ret.value = (*_regs.strTo8bit[operand.name])();
		return ret;
	}

	// not immidiate, gb nned to read mem location from mem through mbc
	ret.isMemoryLocation = true;
	if (operand.name == "a16")
	{
		ret.address = getMemoryLocation16bit();
		ret.value = getMemoryLocation(ret.address);
		return ret;
	}
	else if (operand.name == "HL" || operand.name == "DE" || operand.name == "BC")
	{
		ret.address = (*_regs.strTo16bit[operand.name])();
		ret.value = getMemoryLocation(ret.address);
		return ret;
	}

	// special ld instruction from here [a8] or a [c]
	specialLdMove = true;
	if (operand.name == "a8")
	{
		ret.address = 0; // no adress here, immidiate value
		ret.value = getMemoryLocation();
		return ret;
	}
	ret.address = (*_regs.strTo8bit[operand.name])();
	ret.value = getMemoryLocation(ret.address);
	return ret;
	
	//throw GeneralException("Invalid operand name, or a request to access the F register [get8BitOperand]",UNKNWON_OPERAND);
}

operandReturn<uint8_t> Memory::get8BitOperand(const std::string operand)
{
	specialLdMove = false;
	operandReturn<uint8_t> ret;
	ret.isRegister = operand;
	ret.value = (*_regs.strTo8bit[operand])();
	return ret;
}

operandReturn<uint16_t> Memory::get16BitOperand(const operandElementHolder& operand)
{
	operandReturn<uint16_t> ret;
	if (operand.immediate)
	{
		if (operand.name == "n16" || operand.name == "a16")
		{
			ret.value = getMemoryLocation16bit();
			return ret;
		}
		ret.isRegister = operand.name;
		ret.value = (*_regs.strTo16bit[operand.name])();
		return ret;
	}

	if (operand.name == "a16")
	{
		ret.isMemoryLocation = true;
		ret.address = getMemoryLocation16bit();
		ret.value = getMemoryLocation16bit(ret.address);
		return ret;
	}
	
	throw GeneralException("Invalid operand name [get16BitOperand]", UNKNWON_OPERAND);
}

MBC* Memory::getMBC()
{
	return _mbc;
}
