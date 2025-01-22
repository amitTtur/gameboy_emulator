#include "ShiftRotate8bit.h"

/*********************************
*
*
*       OPCODE REMINDER
*       [Z] [N] [H] [C]
*
*
*********************************/

int ShiftRotate8bit::run()
{
    auto it = _strToFunc.find(_currentOpcode.mnemonic);

    if (it != _strToFunc.end()) {
        // Call the function pointed to by the iterator
        (this->*(it->second))(); // run the opcode
        return _currentOpcode.cycles;
    }
    
    throw GeneralException("Unsupported opcode: " + _currentOpcode.mnemonic + ", had appeared at [run-> arithmetic8Bit]", UNKNOWN_OPCODE);
}

void ShiftRotate8bit::printOpcodeMnemonic() const
{
    std::cout << _currentOpcode.mnemonic << " ";
    for (int i = 0; i < _currentOpcode.operands.size(); i++)
    {
        std::cout << _currentOpcode.operands[i].name << " ";
    }
    std::cout << std::endl;
}

void ShiftRotate8bit::RLC()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = (operand.value << 1) | (operand.value >> 7);

    regs.CF((operand.value >> 7) & 1);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::RRC()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = (operand.value >> 1) | (operand.value << 7);

    regs.CF(operand.value & 1);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }

}

void ShiftRotate8bit::RL()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = (operand.value << 1) | regs.CF();

    regs.CF((operand.value >> 7) & 1);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }

}

void ShiftRotate8bit::RR()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = (regs.CF() << 7) | (operand.value >> 1);

    regs.CF(operand.value & 1);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::SLA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = operand.value << 1;

    regs.CF((operand.value >> 7) & 1);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }

}

void ShiftRotate8bit::SRA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = operand.value << 7;

    regs.CF(operand.value & 1);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::SWAP()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    int lsb = (operand.value & 0xF) << 4;
    int msb = (operand.value & 0xF0) >> 4;

    uint8_t result = msb + lsb;

    regs.CF(0);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::SRL()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode.operands[0]);

    regs.CF(operand.value & 1);

    uint8_t result = operand.value >> 1;

    regs.NF(0);
    regs.HF(0);
    regs.ZF(result == 0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::BIT()
{
    RegisterFile& regs = _mem.getsRegs();

    int operand1 = std::stoi(_currentOpcode.operands[0].name);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    regs.NF(0);
    regs.HF(1);

    regs.ZF(~((operand2.value >> operand1) & 1));

}

void ShiftRotate8bit::RES()
{
    RegisterFile& regs = _mem.getsRegs();

    int operand1 = std::stoi(_currentOpcode.operands[0].name);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    uint8_t result = (operand2.value & ~(1 << operand1)) | (0 << operand1);

    if (operand2.isMemoryLocation)
    {
        _mem.getMBC()->write(operand2.address, result);
    }
    else
    {
        *regs.strTo8bit[operand2.isRegister] = result;
    }
}

void ShiftRotate8bit::SET()
{
    RegisterFile& regs = _mem.getsRegs();

    int operand1 = std::stoi(_currentOpcode.operands[0].name);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    uint8_t result = (operand2.value & ~(1 << operand1)) | (1 << operand1);

    if (operand2.isMemoryLocation)
    {
        _mem.getMBC()->write(operand2.address, result);
    }
    else
    {
        *regs.strTo8bit[operand2.isRegister] = result;
    }
}

void ShiftRotate8bit::RLCA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand("A");

    uint8_t result = operand.value << 1 | ((operand.value >> 7) & 1);

    regs.CF(((operand.value >> 7) & 1));
    regs.NF(0);
    regs.HF(0);
    regs.ZF(0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::RRCA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand("A");

    uint8_t result = operand.value >> 1 | ((operand.value & 1) << 7);

    regs.CF(operand.value & 1);
    regs.NF(0);
    regs.HF(0);
    regs.ZF(0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::RLA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand("A");

    uint8_t result = operand.value << 1 | regs.CF();

    regs.CF(((operand.value >> 7) & 1)); //not sure
    regs.NF(0);
    regs.HF(0);
    regs.ZF(0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}

void ShiftRotate8bit::RRA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand("A");

    uint8_t result = operand.value >> 1 | ((regs.CF() & 1) << 7);

    regs.CF(operand.value & 1); //not sure
    regs.NF(0);
    regs.HF(0);
    regs.ZF(0);

    if (operand.isMemoryLocation)
    {
        _mem.getMBC()->write(operand.address, result);
    }
    else
    {
        *regs.strTo8bit[operand.isRegister] = result;
    }
}
