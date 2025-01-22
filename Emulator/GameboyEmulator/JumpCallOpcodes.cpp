#include "JumpCallOpcodes.h"

int JumpCallOpcodes::run()
{
    auto it = _strToFunc.find(_currentOpcode.mnemonic);

    if (it != _strToFunc.end()) {

        return (this->*(it->second))(); // Call the function pointed to by the iterator
    }

    throw GeneralException("Unsupported opcode: " + _currentOpcode.mnemonic + ", had appeared at [run-> jumpCallOpcodes]", UNKNOWN_OPCODE);
}

void JumpCallOpcodes::printOpcodeMnemonic() const
{
    std::cout << _currentOpcode.mnemonic << " ";
    for (int i = 0; i < _currentOpcode.operands.size(); i++)
    {
        std::cout << _currentOpcode.operands[i].name << " ";
    }
    std::cout << std::endl;
}

int JumpCallOpcodes::JR()
{
    RegisterFile& regs = _mem.getsRegs();
    if (_currentOpcode.operands.size() == 1)
    {
        operandReturn<uint8_t> tmp = _mem.get8BitOperand(_currentOpcode.operands[0]);
        int8_t signedOffset = static_cast<int8_t>(tmp.value);
        regs.PC += static_cast<int>(signedOffset);
        return 8;
    }

    std::string operandName = _currentOpcode.operands[0].name;
    operandReturn<uint8_t> tmp = _mem.get8BitOperand(_currentOpcode.operands[1]);
    int8_t signedOffset = static_cast<int8_t>(tmp.value);

    if ((operandName == "NZ" && !regs.ZF()) ||
        (operandName == "Z" && regs.ZF()) ||
        (operandName == "NC" && !regs.CF()) ||
        (operandName == "C" && regs.CF())) {
        regs.PC += static_cast<int>(signedOffset);
    }

    return 12;
}


int JumpCallOpcodes::RET()
{
    RegisterFile& regs = _mem.getsRegs();

    auto performReturn = [&]() {
        if (_currentOpcode.mnemonic == "RETI")
        {
            _mem.IME(1);
        }

        regs.PC = _mem.getMemoryLocation16bit(regs.SP);
        regs.SP += 2;
        };

    // Unconditional return
    if (_currentOpcode.operands.empty())
    {
        performReturn();
        return 16;
    }

    std::string operandName = _currentOpcode.operands[0].name;
    if ((operandName == "NZ" && !regs.ZF()) ||
        (operandName == "Z" && regs.ZF()) ||
        (operandName == "NC" && !regs.CF()) ||
        (operandName == "C" && regs.CF()))
    {
        performReturn();
        return 20;
    }

    return 8;
}

int JumpCallOpcodes::JP()
{
    RegisterFile& regs = _mem.getsRegs();


    auto performSet = [&](bool n) { // LAMBDA!!!
        operandReturn<uint16_t> tmp = _mem.get16BitOperand(_currentOpcode.operands[n]);
        regs.PC = tmp.value;
        };

    // Unconditional return
    if (_currentOpcode.operands.size() == 1)
    {
        if (_currentOpcode.operands[0].name == "HL")
        {
            regs.PC = regs.HL;
            return 4;
        }
        performSet(0);
        return 16;
    }

    std::string operandName = _currentOpcode.operands[0].name;
    if ((operandName == "NZ" && !regs.ZF()) ||
        (operandName == "Z" && regs.ZF()) ||
        (operandName == "NC" && !regs.CF()) ||
        (operandName == "C" && regs.CF()))
    {
        return 16;
        performSet(1);
    }
    else
    {
        // still need to get the operand
        operandReturn<uint16_t> tmp = _mem.get16BitOperand(_currentOpcode.operands[1]);
    }

    return 12;
}

int JumpCallOpcodes::CALL()
{
    RegisterFile& regs = _mem.getsRegs();

    auto performCall = [&](bool n) { // LAMBDA!!!

        // push the current PC
        // add the pc the opcode after this one
        uint16_t pcAfterCurrentOpcode = regs.PC + 2;
        //
        // little endian push
        regs.SP-=1;
        _mem.getMBC()->write(regs.SP, (pcAfterCurrentOpcode >> 8) & 0xFF); // High byte
        regs.SP-=1;
        _mem.getMBC()->write(regs.SP, pcAfterCurrentOpcode & 0xFF);
       
        regs.PC = _mem.get16BitOperand(_currentOpcode.operands[n]).value;
        };

    if (_currentOpcode.operands.size() == 1)
    {
        performCall(0);
        return 24;
    }

    std::string operandName = _currentOpcode.operands[0].name;
    if ((operandName == "NZ" && !regs.ZF()) ||
        (operandName == "Z" && regs.ZF()) ||
        (operandName == "NC" && !regs.CF()) ||
        (operandName == "C" && regs.CF()))
    {
        performCall(1);
        return 24;
    }
    else
    {
        // skip the operand
        regs.PC += 2;
    }

    return 12;
}

int JumpCallOpcodes::RST()
{
    RegisterFile& regs = _mem.getsRegs();

    // bibicly accurate push
    regs.SP -= 1;
    _mem[regs.SP] = (regs.PC & 0xFF00) >> 8; // high byte
    regs.SP -= 1;
    _mem[regs.SP] = regs.PC & 0xFF; // low byte

    regs.PC = std::stoi(_currentOpcode.operands[0].name.substr(1), nullptr, 16);

    return 4;
}
