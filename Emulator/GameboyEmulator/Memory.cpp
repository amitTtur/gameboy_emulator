#include "Memory.h"

Memory::Memory() : _IE("IE", _mem[INTERRUPT_ENABLE_LOC]), _IF("IF", _mem[INTERRUPT_FLAG_LOC]) 
{
	_IME = 1;
}

Memory::~Memory()
{
	delete _mbc;
}

void Memory::init(const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag)
{
	switch (cartridgeView::getType(MBC::getRomHeaders(romPath)))
	{
	case mbc1:
	{
		_mbc = new MBC1(_mem, romPath,saveFolderPath,saveFlag);
		break;
	}
	case romOnly:
	{
		_mbc = new NoMBC(_mem, romPath, saveFolderPath, saveFlag);
		break;
	}
	case mbc2:
	{
		_mbc = new MBC2(_mem, romPath, saveFolderPath, saveFlag);
		break;
	}
	case mbc3:
	{
		_mbc = new MBC3(_mem, romPath, saveFolderPath, saveFlag);
		break;
	}
	case mbc5:
	{
		_mbc = new MBC5(_mem, romPath, saveFolderPath, saveFlag);
		break;
	}
	default:
	{
		throw GeneralException("The selected mbc type isn't supported.", MBC_NOT_SUPPORTED);
	}
	}
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

	if (globalVars::haltBug())
	{
		globalVars::haltBug(false);
	}
	else
	{
		_regs.PC += 1;
	}
	return temp;
}

uint16_t Memory::getMemoryLocation16bit() {
	uint8_t lowByte = _mbc->read(_regs.PC);          // Read the high byte
	uint8_t highByte = _mbc->read(_regs.PC + 1);     // Read the low byte

	// Combine the two bytes into a 16-bit value (little-endian)
	uint16_t tmp = (highByte << 8) | lowByte;

	if (globalVars::haltBug())
	{
		globalVars::haltBug(false);
		_regs.PC += 1;
	}
	else
	{
		_regs.PC += 2;
	}

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
	globalVars::specialLdMove(false);
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
	globalVars::specialLdMove(true);
	if (operand.name == "a8")
	{
		ret.address = 0; // no adress here, immidiate value
		ret.value = getMemoryLocation();
		return ret;
	}
	ret.address = 0;
	ret.value = (*_regs.strTo8bit[operand.name])();
	return ret;
	
	//throw GeneralException("Invalid operand name, or a request to access the F register [get8BitOperand]",UNKNWON_OPERAND);
}

operandReturn<uint8_t> Memory::get8BitOperand(const std::string operand)
{
	globalVars::specialLdMove(false);
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

uint8_t Memory::int_Vblank() { return IF() & 1; } //bit 0
uint8_t Memory::int_LCDCStatus() { return (IF() >> 1) & 1; } //bit 1
uint8_t Memory::int_timerOverflow() { return (IF() >> 2) & 1; } //bit 2
uint8_t Memory::int_serialTransfer() { return (IF() >> 3) & 1; } //bit 3
uint8_t Memory::int_Joypad() { return (IF() >> 4) & 1; } //bit 4

void Memory::int_Vblank(uint8_t val) { IF() = (IF() & ~1 | val); } //bit 0
void Memory::int_LCDCStatus(uint8_t val) { IF() = (IF() & ~(1 << 1)) | (val << 1); } //bit 1
void Memory::int_timerOverflow(uint8_t val) { IF() = (IF() & ~(1 << 2)) | (val << 2); }  //bit 2
void Memory::int_serialTransfer(uint8_t val) { IF() = (IF() & ~(1 << 3)) | (val << 3); }  //bit 3
void Memory::int_Joypad(uint8_t val) { IF() = (IF() & ~(1 << 4)) | (val << 4); } //bit 4

//interrupts enable
uint8_t Memory::intEnable_Vblank() { return IE() & 1; } //bit 0
uint8_t Memory::intEnable_LCDCStatus() { return (IE() >> 1) & 1; } //bit 1
uint8_t Memory::intEnable_timerOverflow() { return (IE() >> 2) & 1; } //bit 2
uint8_t Memory::intEnable_serialTransfer() { return (IE() >> 3) & 1; } //bit 3
uint8_t Memory::intEnable_Joypad() { return (IE() >> 4) & 1; } //bit 4

void Memory::intEnable_Vblank(uint8_t val) { IE() = (IE() & ~1 | val); } //bit 0
void Memory::intEnable_LCDCStatus(uint8_t val) { IE() = (IE() & ~(1 << 1)) | (val << 1); } //bit 1
void Memory::intEnable_timerOverflow(uint8_t val) { IE() = (IE() & ~(1 << 2)) | (val << 2); }  //bit 2
void Memory::intEnable_serialTransfer(uint8_t val) { IE() = (IE() & ~(1 << 3)) | (val << 3); }  //bit 3
void Memory::intEnable_Joypad(uint8_t val) { IE() = (IE() & ~(1 << 4)) | (val << 4); } //bit 4
