#include "Load8Bit.h"

Load8Bit::Load8Bit(Memory& mem_ref, OpcodeElementHolder* currentOpcode) : Opcode(mem_ref, currentOpcode) {}

int Load8Bit::run()
{
	LD(); // run the opcode

	return _currentOpcode->cycles;
}

void Load8Bit::printOpcodeMnemonic() const
{
	std::cout << _currentOpcode->mnemonic << " ";
	for (int i = 0; i < _currentOpcode->operands.size(); i++)
	{
		std::cout << _currentOpcode->operands[i].name << " ";
	}
	std::cout << std::endl;
}

void Load8Bit::LD()
{
	RegisterFile& regs = _mem.getsRegs();
	uint16_t memPlace = 0; // case special move;
	// Retrieve operands
	operandReturn<uint8_t> operand1 = _mem.get8BitOperand(_currentOpcode->operands[0]);

	// this is basicily the ldh instruction check
	// need to load register A into 0xff00 + the value specified at [c] or [a8]
	if (globalVars::specialLdMove())
	{
		memPlace = 0xFF00 + operand1.value;
		// write 8 bit not 16
		//basicily what it does: _mem[memPlace] = register A;
		_mem.getMBC()->write(memPlace, regs.A);
		globalVars::specialLdMove(false);
		return;
	}


	operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);
	// need to load into register A the value: 0xff00 + the value specified at [c] or [a8]
	if (globalVars::specialLdMove())
	{
		memPlace = 0xFF00 + operand2.value;
		// write 8 bit not 16
		//basicily what it does: register A = _mem[memPlace];
		regs.A = _mem.getMBC()->read(memPlace);
		globalVars::specialLdMove(false);
		return;
	}

	// from here is a normal ld instrcution, previous was the ldh
	if (operand1.isMemoryLocation)
	{
		if (operand1.address == 16964)
		{
			std::cout<<std::endl;
		}
		_mem.getMBC()->write(operand1.address, operand2.value); // [register/a16] = value
	}
	else // is a register
	{
		*regs.strTo8bit[operand1.isRegister] = operand2.value;
	}

	// Increment/Decrement Logic
	// this cases will always be true only for the HL register
	// this is the only register who has an increase decrease
	// option in ld
	if (_currentOpcode->operands[0].increment) regs.HL += 1;
	else if (_currentOpcode->operands[0].decrement) regs.HL -= 1;

	if (_currentOpcode->operands[1].increment) regs.HL += 1;
	else if (_currentOpcode->operands[1].decrement) regs.HL -= 1;

}
