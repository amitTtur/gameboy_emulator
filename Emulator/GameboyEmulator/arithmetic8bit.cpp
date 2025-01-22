#include "arithmetic8bit.h"


/*********************************
*
*
*       OPCODE REMINDER
*       [Z] [N] [H] [C]
*
*
*********************************/

int Arithmetic8bit::run()
{

    auto it = _strToFunc.find(_currentOpcode.mnemonic);

    if (it != _strToFunc.end()) {
        // Call the function pointed to by the iterator
        (this->*(it->second))(); // run the opcode
        return _currentOpcode.cycles;
    }
    
    throw GeneralException("Unsupported opcode: " + _currentOpcode.mnemonic + ", had appeared at [run-> arithmetic8Bit]", UNKNOWN_OPCODE);
}

void Arithmetic8bit::printOpcodeMnemonic() const
{
    std::cout << _currentOpcode.mnemonic << " ";
    for (int i = 0; i < _currentOpcode.operands.size(); i++)
    {
        std::cout << _currentOpcode.operands[i].name << " ";
    }
    std::cout << std::endl;
}

void Arithmetic8bit::OR()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    // no need will always be the register A
    // operandReturn<uint8_t> operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    uint8_t result = regs.A | operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(false);    // OR operations always clear the N flag
    regs.CF(false);    // OR operations always clear the C flag
    regs.HF(false);     // OR operations always set the H flag

    // Update the first operand with the result
    regs.A = result;
}

void Arithmetic8bit::INC()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    operandReturn<uint8_t> operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = operand1.value + 1;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(false);
    regs.HF((operand1.value & 0xF) == 0xF);     // OR operations always set the H flag

    // Update the first operand with the result
    if (operand1.isMemoryLocation)
    {
        _mem.getMBC()->write(operand1.address, result);
    }
    else
    {
        *regs.strTo8bit[operand1.isRegister] = result;
    }
}

void Arithmetic8bit::DEC()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    operandReturn<uint8_t> operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);

    uint8_t result = operand1.value - 1;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(true); 
    regs.HF((operand1.value & 0xF) == 0);  

    // Update the first operand with the result
    if (operand1.isMemoryLocation)
    {
        _mem.getMBC()->write(operand1.address, result);
    }
    else
    {
        *regs.strTo8bit[operand1.isRegister] = result;
    }
}

void Arithmetic8bit::ADD()
{
    // Access registers
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    // will always be resiter A
    // uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    // Perform addition
    uint8_t result = regs.A + operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.CF((regs.A + operand2.value) > 0xFF);  // Carry flag

    // Half-carry flag: Carry from bit 3 to 4
    // clears the left 4 bits for both the operands
    // the sees if a addition between them surpassses 0xF
    regs.HF(((regs.A & 0xF) + (operand2.value & 0xF)) > 0xF);
    regs.NF(false);                                     // Add operations always clear the N flag

    // Update the first operand with the result
    regs.A = result;
}

void Arithmetic8bit::SUB()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    // reg A
    // uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    // Perform subtraction
    uint8_t result = regs.A - operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag

    // Half-carry flag: Carry from bit 3 to 4
    regs.HF((regs.A & 0xF) < (operand2.value & 0xF));
    regs.NF(true);                                     // sub operations always set the N flag

    regs.CF(regs.A < operand2.value); // Carry flag

    // Update the first operand with the result
    regs.A = result;

}

// AMIT (im really offended :( ) is now goin over documentation, there is a scenario when both the
// regs are A the the carry is untouched. keep in note. 
void Arithmetic8bit::SBC()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    // Perform subtraction
    uint8_t result = regs.A - operand2.value - regs.CF();

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag

    // Half-carry flag: Carry from bit 3 to 4
    regs.HF((regs.A & 0xF) < (operand2.value & 0xF) + regs.CF());
    regs.NF(true);                                     // sub operations always set the N flag

    regs.CF(regs.A < (operand2.value + regs.CF())); // carry flag

    // Update the first operand with the result
    regs.A = result;
}

void Arithmetic8bit::ADC()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    // uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    // Perform subtraction
    uint8_t result = regs.A + operand2.value + regs.CF();

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag

    // Half-carry flag: Carry from bit 3 to 4
    // clears the left 4 bits for both the operands
    // the sees if a addition between them surpassses 0xF
    regs.HF(((regs.A & 0xF) + (operand2.value & 0xF) + regs.CF()) > 0xF);
    regs.NF(false);                                     // Add operations always clear the N flag

    regs.CF((regs.A + operand2.value + regs.CF()) > 0xFF); // Carry flag

    // Update the first operand with the result
    regs.A = result;
}

void Arithmetic8bit::AND()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    uint8_t result = regs.A & operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(false);    // and operations always clear the N flag
    regs.CF(false);    // and operations always clear the C flag
    regs.HF(true);     // and operations always set the H flag

    // Update the first operand with the result
    regs.A = result;
}

void Arithmetic8bit::XOR()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    uint8_t result = regs.A ^ operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(false);    // xor operations always clear the N flag
    regs.CF(false);    // xor operations always clear the C flag
    regs.HF(false);     // xor operations always set the H flag

    // Update the first operand with the result
    regs.A = result;

}

void Arithmetic8bit::CP()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode.operands[1]);

    // Perform subtraction
    uint8_t result = regs.A - operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag

    // Half-carry flag: Carry from bit 3 to 4
    regs.HF((regs.A & 0xF) < (operand2.value & 0xF));
    regs.NF(true);                                     // sub operations always set the N flag

    regs.CF(regs.A < operand2.value); // Carry flag

}

void Arithmetic8bit::DAA()
{
    RegisterFile& regs = _mem.getsRegs();

    uint8_t result = regs.A;
    uint8_t correction = regs.CF() ? 0x60 : 0x00;
    if (regs.HF() || (!regs.NF() && ((result & 0x0F) > 9))) {
        correction |= 0x06;
    }
    if (regs.CF() || (!regs.NF() && (result > 0x99))) {
        correction |= 0x60;
    }
    if (regs.NF()) {
        result -= correction;
    }
    else {
        result += correction;
    }
    if (((correction << 2) & 0x100) != 0) {
        regs.CF(true);
    }

    regs.HF(false);
    regs.ZF(result == 0);
    regs.A = result;


}

void Arithmetic8bit::CPL()
{
    RegisterFile& regs = _mem.getsRegs();

    ~regs.A;
    regs.HF(true);
    regs.NF(true);

}

void Arithmetic8bit::SCF()
{
    RegisterFile& regs = _mem.getsRegs();
    regs.CF(true);
    regs.HF(false);
    regs.NF(false);

  
}

void Arithmetic8bit::CCF()
{
    RegisterFile& regs = _mem.getsRegs();
    regs.CF(!regs.CF());
    regs.HF(false);
    regs.NF(false);

}
