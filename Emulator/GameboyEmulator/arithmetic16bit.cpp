#include "arithmetic16bit.h"

/*********************************
*
*
*       OPCODE REMINDER
*       [Z] [N] [H] [C]
*
*
*********************************/

Arithmetic16bit::Arithmetic16bit(Memory& mem_ref, OpcodeElementHolder* currentOpcode) : Opcode(mem_ref, currentOpcode)
{
    // Initialize the map with opcode strings as keys and function pointers as values
    _strToFunc["ADD"] = &Arithmetic16bit::ADD;
    _strToFunc["INC"] = &Arithmetic16bit::INC;
    _strToFunc["DEC"] = &Arithmetic16bit::DEC;
}

int Arithmetic16bit::run()
{
    auto it = _strToFunc.find(_currentOpcode->mnemonic);

    if (it != _strToFunc.end()) {
        // Call the function pointed to by the iterator
        (this->*(it->second))(); // run the opcode
        return _currentOpcode->cycles;
    }
    
    throw GeneralException("Unsupported opcode: " + _currentOpcode->mnemonic + ", had appeared at [run-> arithmetic16Bit]", UNKNOWN_OPCODE);
}

void Arithmetic16bit::printOpcodeMnemonic() const
{
    std::cout << _currentOpcode->mnemonic << " ";
    for (int i = 0; i < _currentOpcode->operands.size(); i++)
    {
        std::cout << _currentOpcode->operands[i].name << " ";
    }
    std::cout << std::endl;
}

void Arithmetic16bit::INC()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint16_t> operand = _mem.get16BitOperand(_currentOpcode->operands[0]);

    // will always be a register
    *regs.strTo16bit[operand.isRegister] += 1;
}

void Arithmetic16bit::DEC()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint16_t> operand = _mem.get16BitOperand(_currentOpcode->operands[0]);

    *regs.strTo16bit[operand.isRegister] -= 1;
}

void Arithmetic16bit::ADD()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode->operands[0]);
    
    

    //if something is wrong its probably here.
    //check here first

    if (_currentOpcode->operands[0].name == "SP")
    {
        // this specific case asks dor a signed operand, uint8_t is unsigned
        int operand2 = static_cast<int8_t>(_mem.getMemoryLocation());

        regs.CF(((operand1.value & 0xFF) + (operand2 & 0xFF)) > 0xFF);
        regs.HF(((operand1.value & 0xF) + (operand2 & 0xF)) > 0xF);
        
        regs.SP += operand2;
        regs.ZF(false); // based on documentation, only this one should reset the zf flag
    }
    else
    {
        operandReturn<uint16_t> operand2 = _mem.get16BitOperand(_currentOpcode->operands[1]);

        regs.CF(operand1.value + operand2.value > 0xffff);
        regs.HF((operand1.value & 0x0fff) + (operand2.value & 0x0fff) > 0x0fff);

        regs.HL += operand2.value;
    }
    regs.NF(false);
}
