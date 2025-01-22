#include "Load16Bit.h"

int Load16Bit::run()
{
	auto it = _strToFunc.find(_currentOpcode.mnemonic);

	if (it != _strToFunc.end()) {

		// Call the function pointed to by the iterator
		(this->*(it->second))(); // run the opcode
		return _currentOpcode.cycles;
	}

	throw GeneralException("Unsupported opcode: " + _currentOpcode.mnemonic + ", had appeared at [run-> load16bit]", UNKNOWN_OPCODE);
}

void Load16Bit::printOpcodeMnemonic() const
{
	std::cout << _currentOpcode.mnemonic << " ";
	for (int i = 0; i < _currentOpcode.operands.size(); i++)
	{
		std::cout << _currentOpcode.operands[i].name << " ";
	}
	std::cout << std::endl;
}

void Load16Bit::LD()
{
	RegisterFile& regs = _mem.getsRegs();

	if (_currentOpcode.operands.size() == 3)
	{
		specialLD();
		return;
	}

	// Retrieve operands
	operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode.operands[0]);
	operandReturn<uint16_t> operand2 = _mem.get16BitOperand(_currentOpcode.operands[1]);

	if (operand1.isMemoryLocation)
	{
		_mem.getMBC()->write16(operand1.address, operand2.value);
	}
	else // is a register
	{
		*regs.strTo16bit[operand1.isRegister] = operand2.value;
	} 
}

void Load16Bit::POP()
{
	RegisterFile& regs = _mem.getsRegs();

	operandReturn<uint16_t>  operand1 = _mem.get16BitOperand(_currentOpcode.operands[0]);

	uint16_t val = 0;
	val += _mem.getMBC()->read(regs.SP);
	regs.SP += 1;
	val += _mem.getMBC()->read(regs.SP) << 8; // High byte
	regs.SP += 1;
	

	*regs.strTo16bit[operand1.isRegister] = val;
}

void Load16Bit::PUSH()
{
	RegisterFile& regs = _mem.getsRegs();

	operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode.operands[0]);

	regs.SP -= 1;
	_mem.getMBC()->write(regs.SP, (operand1.value >> 8) & 0xFF); // High byte
	regs.SP -= 1;
	_mem.getMBC()->write(regs.SP, operand1.value & 0xFF);
}

void Load16Bit::specialLD()
{

	RegisterFile& regs = _mem.getsRegs();

	//operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode.operands[0]); //hl
	//operandReturn<uint16_t> operand2 = _mem.get16BitOperand(_currentOpcode.operands[1]); // sp
	operandReturn<uint8_t> operand3 = _mem.get8BitOperand(_currentOpcode.operands[2]); // e

	// static case from uint8 (unsigned) to int (signed)
	uint32_t result = static_cast<int8_t>(operand3.value) + regs.SP;
	regs.HL = result & 0xFFFF;
	regs.NF(false);
	regs.ZF(false);
	regs.HF(((regs.SP & 0x0F) + (operand3.value & 0x0F)) > 0x0F);

	regs.CF(((regs.SP & 0xFF) + (operand3.value & 0xFF)) > 0xFF);

}
