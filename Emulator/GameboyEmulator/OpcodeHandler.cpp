#include "OpcodeHandler.h"

OpcodeHandler::OpcodeHandler(Memory& memoryReferance) : _mem(memoryReferance)
{
	_prefix = false;
    initStrToFunc();
	initializeOpcodes();
}

int OpcodeHandler::run(uint8_t opcode)
{

    if (_prefix)
    {
        _prefix = false; 
        _currentOpcode = &_CBopcodes[opcode].second;
        return (this->*(_CBopcodes[opcode].first))();
    }
    else
    {
        _currentOpcode = &_opcodes[opcode].second;
        return (this->*(_opcodes[opcode].first))();
    }
}

void OpcodeHandler::setPrefix(const bool& val)
{
    _prefix = val;
}

bool OpcodeHandler::getPrefix() const
{
	return _prefix;
}

OpcodeElementHolder* OpcodeHandler::getCurrentOpcode()
{
    return _currentOpcode;
}

void OpcodeHandler::initStrToFunc()
{
    _mnemonicToFunc["ADD"] = &OpcodeHandler::ADD;
    _mnemonicToFunc["SUB"] = &OpcodeHandler::SUB;
    _mnemonicToFunc["AND"] = &OpcodeHandler::AND;
    _mnemonicToFunc["OR"] = &OpcodeHandler::OR;
    _mnemonicToFunc["INC"] = &OpcodeHandler::INC;
    _mnemonicToFunc["DEC"] = &OpcodeHandler::DEC;
    _mnemonicToFunc["SBC"] = &OpcodeHandler::SBC;
    _mnemonicToFunc["ADC"] = &OpcodeHandler::ADC;
    _mnemonicToFunc["XOR"] = &OpcodeHandler::XOR;
    _mnemonicToFunc["CP"] = &OpcodeHandler::CP;
    _mnemonicToFunc["DAA"] = &OpcodeHandler::DAA;
    _mnemonicToFunc["CPL"] = &OpcodeHandler::CPL;
    _mnemonicToFunc["SCF"] = &OpcodeHandler::SCF;
    _mnemonicToFunc["CCF"] = &OpcodeHandler::CCF;
    _mnemonicToFunc["JR"] = &OpcodeHandler::JR;
    _mnemonicToFunc["RET"] = &OpcodeHandler::RET;
    _mnemonicToFunc["RETI"] = &OpcodeHandler::RET;
    _mnemonicToFunc["JP"] = &OpcodeHandler::JP;
    _mnemonicToFunc["CALL"] = &OpcodeHandler::CALL;
    _mnemonicToFunc["RST"] = &OpcodeHandler::RST;
    _mnemonicToFunc["LD"] = &OpcodeHandler::LD;
    _mnemonicToFunc["LDH"] = &OpcodeHandler::LD;
    _mnemonicToFunc["POP"] = &OpcodeHandler::POP;
    _mnemonicToFunc["PUSH"] = &OpcodeHandler::PUSH;
    _mnemonicToFunc["NOP"] = &OpcodeHandler::NOP;
    _mnemonicToFunc["STOP"] = &OpcodeHandler::STOP;
    _mnemonicToFunc["HALT"] = &OpcodeHandler::HALT;
    _mnemonicToFunc["PREFIX"] = &OpcodeHandler::PREFIX;
    _mnemonicToFunc["DI"] = &OpcodeHandler::DI;
    _mnemonicToFunc["EI"] = &OpcodeHandler::EI;
    _mnemonicToFunc["RLC"] = &OpcodeHandler::RLC;
    _mnemonicToFunc["RRC"] = &OpcodeHandler::RRC;
    _mnemonicToFunc["RL"] = &OpcodeHandler::RL;
    _mnemonicToFunc["RR"] = &OpcodeHandler::RR;
    _mnemonicToFunc["SLA"] = &OpcodeHandler::SLA;
    _mnemonicToFunc["SRA"] = &OpcodeHandler::SRA;
    _mnemonicToFunc["SWAP"] = &OpcodeHandler::SWAP;
    _mnemonicToFunc["SRL"] = &OpcodeHandler::SRL;
    _mnemonicToFunc["BIT"] = &OpcodeHandler::BIT;
    _mnemonicToFunc["RES"] = &OpcodeHandler::RES;
    _mnemonicToFunc["SET"] = &OpcodeHandler::SET;
    _mnemonicToFunc["RLCA"] = &OpcodeHandler::RLCA;
    _mnemonicToFunc["RRCA"] = &OpcodeHandler::RRCA;
    _mnemonicToFunc["RLA"] = &OpcodeHandler::RLA;
    _mnemonicToFunc["RRA"] = &OpcodeHandler::RRA;
}

void OpcodeHandler::initializeOpcodes()
{
    json j = getOpcodesJsonContents();
    int i = 0;
    int k = 0;

    std::map<int, OpcodeElementHolder> uniqueName;
    for (auto& item : j["unprefixed"].items())
    {
        OpcodeElementHolder tmp;
        auto& values = item.value();

        if (values.contains("mnemonic"))
        {
            tmp.mnemonic = values["mnemonic"];
        }
        else {
            throw GeneralException("opcode factory initialization, opcode didn't have a name", NO_NAME_FOUND);
        }

        if (values.contains("bytes"))
        {
            tmp.bytes = values["bytes"];
        }
        else { tmp.bytes = 1; }

        if (values.contains("cycles"))
        {
            tmp.cycles = values["cycles"][0];
        }
        else { tmp.cycles = 4; }

        if (values.contains("immediate"))
        {
            tmp.immediate = values["immediate"];
        }
        else { tmp.immediate = true; }

        if (values.contains("operands")) // operands iteration
        {
            for (const auto& item2 : values["operands"])
            {
                operandElementHolder tmp2;
                // Assuming item2 is an object and you want to access its fields
                if (item2.contains("name"))
                {
                    tmp2.name = item2["name"];
                }
                else
                {
                    throw GeneralException("opcode factory initialization, operand didn't have a name", NO_NAME_FOUND);
                }
                if (item2.contains("bytes"))
                {
                    tmp2.bytes = item2["bytes"];
                }
                else { tmp2.bytes = 1; }

                if (item2.contains("immediate"))
                {
                    tmp2.immediate = item2["immediate"];
                }
                else { tmp2.bytes = true; }

                if (item2.contains("decrement"))
                {
                    tmp2.decrement = item2["decrement"];
                }
                else { tmp2.decrement = false; }

                if (item2.contains("increment"))
                {
                    tmp2.increment = item2["increment"];
                }
                else { tmp2.increment = false; }


                tmp.operands.push_back(tmp2);
            }
        }
        _opcodes[i].first = _mnemonicToFunc[tmp.mnemonic];
        _opcodes[i].second = tmp;
        i++;
    }

    for (auto& item : j["cbprefixed"].items())
    {
        OpcodeElementHolder tmp;
        auto& values = item.value();

        if (values.contains("mnemonic"))
        {
            tmp.mnemonic = values["mnemonic"];
        }
        else {
            throw GeneralException("opcode factory initialization, opcode didn't have a name", NO_NAME_FOUND);
        }

        if (values.contains("bytes"))
        {
            tmp.bytes = values["bytes"];
        }
        else { tmp.bytes = 1; }

        if (values.contains("cycles"))
        {
            tmp.bytes = values["cycles"][0];
        }
        else { tmp.cycles = 4; }

        if (values.contains("immediate"))
        {
            tmp.immediate = values["immediate"];
        }
        else { tmp.immediate = true; }

        if (values.contains("operands")) // operands iteration
        {
            for (const auto& item2 : values["operands"])
            {
                operandElementHolder tmp2;
                // Assuming item2 is an object and you want to access its fields
                if (item2.contains("name"))
                {
                    tmp2.name = item2["name"];
                }
                else
                {
                    throw GeneralException("opcode factory initialization, operand didn't have a name", NO_NAME_FOUND);
                }
                if (item2.contains("bytes"))
                {
                    tmp2.bytes = item2["bytes"];
                }
                else { tmp2.bytes = 1; }

                if (item2.contains("immediate"))
                {
                    tmp2.immediate = item2["immediate"];
                }
                else { tmp2.bytes = true; }
                tmp.operands.push_back(tmp2);
            }
        }
        _CBopcodes[k].first = _mnemonicToFunc[tmp.mnemonic];
        _CBopcodes[k].second = tmp;
        k++;
    }
}

inline json OpcodeHandler::getOpcodesJsonContents()
{
    std::ifstream file("opcodes.json");

    if (!file.is_open()) {
        throw GeneralException("opcode factory initialization couldn't open the opcodes.json file...", FILE_WONT_OPEN);
    }

    json j;
    file >> j;
    return j;
}

bool OpcodeHandler::areOperands16bit()
{
    for (const operandElementHolder& operand : _currentOpcode->operands)
    {
        if (operand.immediate)
        {
            if (operand.name == "BC" || operand.name == "AF" || operand.name == "DE" || operand.name == "HL" || operand.name == "SP" || operand.name == "n16") { return true; }
        }
    }

    return false;
}

//arithmatic
int OpcodeHandler::OR()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    // no need will always be the register A
    // operandReturn<uint8_t> operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

    uint8_t result = regs.A | operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(false);    // OR operations always clear the N flag
    regs.CF(false);    // OR operations always clear the C flag
    regs.HF(false);     // OR operations always set the H flag

    // Update the first operand with the result
    regs.A = result;

    return _currentOpcode->cycles;
}

int OpcodeHandler::INC()
{
    RegisterFile& regs = _mem.getsRegs();

    if (areOperands16bit())
    {
        operandReturn<uint16_t> operand = _mem.get16BitOperand(_currentOpcode->operands[0]);
        // will always be a register
        *regs.strTo16bit[operand.isRegister] += 1;
    } 
    else
    {
        // Retrieve operands
        operandReturn<uint8_t> operand1 = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    
    return _currentOpcode->cycles;
}

int OpcodeHandler::DEC()
{
    RegisterFile& regs = _mem.getsRegs();

    if (areOperands16bit())
    {
        operandReturn<uint16_t> operand = _mem.get16BitOperand(_currentOpcode->operands[0]);

        *regs.strTo16bit[operand.isRegister] -= 1;
    }
    else
    {
        // Retrieve operands
        operandReturn<uint8_t> operand1 = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    
    return _currentOpcode->cycles;
}

int OpcodeHandler::ADD()
{
    // Access registers
    RegisterFile& regs = _mem.getsRegs();

    if (areOperands16bit())
    {
        operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode->operands[0]);

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
    else
    {
        operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

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

    
    return _currentOpcode->cycles;
}

int OpcodeHandler::SUB()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    // reg A
    // uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

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
    return _currentOpcode->cycles;

}

int OpcodeHandler::SBC()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

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

    return _currentOpcode->cycles;
}

int OpcodeHandler::ADC()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    // uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

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

    return _currentOpcode->cycles;
}

int OpcodeHandler::AND()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

    uint8_t result = regs.A & operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(false);    // and operations always clear the N flag
    regs.CF(false);    // and operations always clear the C flag
    regs.HF(true);     // and operations always set the H flag

    // Update the first operand with the result
    regs.A = result;

    return _currentOpcode->cycles;
}

int OpcodeHandler::XOR()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

    uint8_t result = regs.A ^ operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag
    regs.NF(false);    // xor operations always clear the N flag
    regs.CF(false);    // xor operations always clear the C flag
    regs.HF(false);     // xor operations always set the H flag

    // Update the first operand with the result
    regs.A = result;

    return _currentOpcode->cycles;
}

int OpcodeHandler::CP()
{
    RegisterFile& regs = _mem.getsRegs();

    // Retrieve operands
    //uint8_t& operand1 = _mem.get8BitOperand(_currentOpcode.operands[0]);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

    // Perform subtraction
    uint8_t result = regs.A - operand2.value;

    // Flag calculations
    regs.ZF(result == 0);  // Zero flag

    // Half-carry flag: Carry from bit 3 to 4
    regs.HF((regs.A & 0xF) < (operand2.value & 0xF));
    regs.NF(true);                                     // sub operations always set the N flag

    regs.CF(regs.A < operand2.value); // Carry flag

    return _currentOpcode->cycles;
}

int OpcodeHandler::DAA()
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

    return _currentOpcode->cycles;
}

int OpcodeHandler::CPL()
{
    RegisterFile& regs = _mem.getsRegs();

    ~regs.A;
    regs.HF(true);
    regs.NF(true);

    return _currentOpcode->cycles;
}

int OpcodeHandler::SCF()
{
    RegisterFile& regs = _mem.getsRegs();
    regs.CF(true);
    regs.HF(false);
    regs.NF(false);

    return _currentOpcode->cycles;
}

int OpcodeHandler::CCF()
{
    RegisterFile& regs = _mem.getsRegs();
    regs.CF(!regs.CF());
    regs.HF(false);
    regs.NF(false);
    return _currentOpcode->cycles;
}

//jumps and calls
int OpcodeHandler::JR()
{
    RegisterFile& regs = _mem.getsRegs();
    if (_currentOpcode->operands.size() == 1)
    {
        operandReturn<uint8_t> tmp = _mem.get8BitOperand(_currentOpcode->operands[0]);
        int8_t signedOffset = static_cast<int8_t>(tmp.value);
        regs.PC += static_cast<int>(signedOffset);
        return 8;
    }

    std::string operandName = _currentOpcode->operands[0].name;
    operandReturn<uint8_t> tmp = _mem.get8BitOperand(_currentOpcode->operands[1]);
    int8_t signedOffset = static_cast<int8_t>(tmp.value);

    if ((operandName == "NZ" && !regs.ZF()) ||
        (operandName == "Z" && regs.ZF()) ||
        (operandName == "NC" && !regs.CF()) ||
        (operandName == "C" && regs.CF())) {
        regs.PC += static_cast<int>(signedOffset);
    }

    return 12;
}

int OpcodeHandler::RET()
{
    RegisterFile& regs = _mem.getsRegs();

    auto performReturn = [&]() {
        if (_currentOpcode->mnemonic == "RETI")
        {
            _mem.IME(1);
        }

        regs.PC = _mem.getMemoryLocation16bit(regs.SP);
        regs.SP += 2;
    };

    // Unconditional return
    if (_currentOpcode->operands.empty())
    {
        performReturn();
        return 16;
    }

    std::string operandName = _currentOpcode->operands[0].name;
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

int OpcodeHandler::JP()
{
    RegisterFile& regs = _mem.getsRegs();


    auto performSet = [&](bool n) { // LAMBDA!!!
        operandReturn<uint16_t> tmp = _mem.get16BitOperand(_currentOpcode->operands[n]);
        regs.PC = tmp.value;
    };

    // Unconditional return
    if (_currentOpcode->operands.size() == 1)
    {
        if (_currentOpcode->operands[0].name == "HL")
        {
            regs.PC = regs.HL;
            return 4;
        }
        performSet(0);
        return 16;
    }

    std::string operandName = _currentOpcode->operands[0].name;
    if ((operandName == "NZ" && !regs.ZF()) ||
        (operandName == "Z" && regs.ZF()) ||
        (operandName == "NC" && !regs.CF()) ||
        (operandName == "C" && regs.CF()))
    {
        performSet(1);
        return 16;
    }
    else
    {
        // still need to get the operand
        operandReturn<uint16_t> tmp = _mem.get16BitOperand(_currentOpcode->operands[1]);
    }

    return 12;
}

int OpcodeHandler::CALL()
{
    RegisterFile& regs = _mem.getsRegs();

    auto performCall = [&](bool n) { // LAMBDA!!!

        // push the current PC
        // add the pc the opcode after this one
        uint16_t pcAfterCurrentOpcode = regs.PC + 2;
        //
        // little endian push
        regs.SP -= 1;
        _mem.getMBC()->write(regs.SP, (pcAfterCurrentOpcode >> 8) & 0xFF); // High byte
        regs.SP -= 1;
        _mem.getMBC()->write(regs.SP, pcAfterCurrentOpcode & 0xFF);

        regs.PC = _mem.get16BitOperand(_currentOpcode->operands[n]).value;
    };

    if (_currentOpcode->operands.size() == 1)
    {
        performCall(0);
        return 24;
    }

    std::string operandName = _currentOpcode->operands[0].name;
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

int OpcodeHandler::RST()
{
    RegisterFile& regs = _mem.getsRegs();

    // bibicly accurate push
    regs.SP -= 1;
    _mem[regs.SP] = (regs.PC & 0xFF00) >> 8; // high byte
    regs.SP -= 1;
    _mem[regs.SP] = regs.PC & 0xFF; // low byte

    regs.PC = std::stoi(_currentOpcode->operands[0].name.substr(1), nullptr, 16);

    return 4;
}

//loads
int OpcodeHandler::LD()
{
    RegisterFile& regs = _mem.getsRegs();

    if (areOperands16bit())
    {

        if (_currentOpcode->operands.size() == 3)
        {
            return specialLD();
        }

        // Retrieve operands
        operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode->operands[0]);
        operandReturn<uint16_t> operand2 = _mem.get16BitOperand(_currentOpcode->operands[1]);

        if (operand1.isMemoryLocation)
        {
            _mem.getMBC()->write16(operand1.address, operand2.value);
        }
        else // is a register
        {
            *regs.strTo16bit[operand1.isRegister] = operand2.value;
        }
    }
    else
    {
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
            return _currentOpcode->cycles;
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
            return _currentOpcode->cycles;
        }

        // from here is a normal ld instrcution, previous was the ldh
        if (operand1.isMemoryLocation)
        {
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

        return _currentOpcode->cycles;
    }

   

}

int OpcodeHandler::POP()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint16_t>  operand1 = _mem.get16BitOperand(_currentOpcode->operands[0]);

    uint16_t val = 0;
    val += _mem.getMBC()->read(regs.SP);
    regs.SP += 1;
    val += _mem.getMBC()->read(regs.SP) << 8; // High byte
    regs.SP += 1;


    *regs.strTo16bit[operand1.isRegister] = val;

    return _currentOpcode->cycles;
}

int OpcodeHandler::PUSH()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode->operands[0]);

    regs.SP -= 1;
    _mem.getMBC()->write(regs.SP, (operand1.value >> 8) & 0xFF); // High byte
    regs.SP -= 1;
    _mem.getMBC()->write(regs.SP, operand1.value & 0xFF);

    return _currentOpcode->cycles;
}

int OpcodeHandler::specialLD()
{

    RegisterFile& regs = _mem.getsRegs();

    //operandReturn<uint16_t> operand1 = _mem.get16BitOperand(_currentOpcode.operands[0]); //hl
    //operandReturn<uint16_t> operand2 = _mem.get16BitOperand(_currentOpcode.operands[1]); // sp
    operandReturn<uint8_t> operand3 = _mem.get8BitOperand(_currentOpcode->operands[2]); // e

    // static case from uint8 (unsigned) to int (signed)
    uint32_t result = static_cast<int8_t>(operand3.value) + regs.SP;
    regs.HL = result & 0xFFFF;
    regs.NF(false);
    regs.ZF(false);
    regs.HF(((regs.SP & 0x0F) + (operand3.value & 0x0F)) > 0x0F);

    regs.CF(((regs.SP & 0xFF) + (operand3.value & 0xFF)) > 0xFF);
    
    return _currentOpcode->cycles;
}

//misc
int OpcodeHandler::NOP()
{
    return _currentOpcode->cycles;
}

int OpcodeHandler::STOP()
{
    //globalVars::running(false);
    std::cout << "stop" << std::endl;
    return _currentOpcode->cycles;
}

int OpcodeHandler::HALT()
{
    if (_mem.IME() == 0 && (_mem.IE() & _mem.IF() & 0x1f) != 0)
    {
        globalVars::haltBug(true);
    }
    else
    {
        globalVars::systemHalted(true);
    }
    return _currentOpcode->cycles;
}

int OpcodeHandler::PREFIX()
{
    _prefix = true;
    return _currentOpcode->cycles;
}

int OpcodeHandler::DI()
{
    _mem.IME(0);
    return _currentOpcode->cycles;
}

int OpcodeHandler::EI()
{
    _mem.IME(1);
    return _currentOpcode->cycles;
}

//shifts and rotates
int OpcodeHandler::RLC()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

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

    return _currentOpcode->cycles;
}

int OpcodeHandler::RRC()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    return _currentOpcode->cycles;
}

int OpcodeHandler::RL()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    return _currentOpcode->cycles;
}

int OpcodeHandler::RR()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    return _currentOpcode->cycles;
}

int OpcodeHandler::SLA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    return _currentOpcode->cycles;
}

int OpcodeHandler::SRA()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

    // 0x10000000 -- bit 7 remains unchanged, 1 shr
    uint8_t result = operand.value >> 1 | operand.value & 0b10000000;

    regs.CF(operand.value & 0x1);
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
    return _currentOpcode->cycles;
}

int OpcodeHandler::SWAP()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    return _currentOpcode->cycles;
}

int OpcodeHandler::SRL()
{
    RegisterFile& regs = _mem.getsRegs();

    operandReturn<uint8_t> operand = _mem.get8BitOperand(_currentOpcode->operands[0]);

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
    return _currentOpcode->cycles;
}

int OpcodeHandler::BIT()
{
    RegisterFile& regs = _mem.getsRegs();

    int operand1 = std::stoi(_currentOpcode->operands[0].name);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

    regs.NF(0);
    regs.HF(1);

    regs.ZF(~((operand2.value >> operand1) & 1));
    return _currentOpcode->cycles;
}

int OpcodeHandler::RES()
{
    RegisterFile& regs = _mem.getsRegs();

    int operand1 = std::stoi(_currentOpcode->operands[0].name);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

    uint8_t result = (operand2.value & ~(1 << operand1)) | (0 << operand1);

    if (operand2.isMemoryLocation)
    {
        _mem.getMBC()->write(operand2.address, result);
    }
    else
    {
        *regs.strTo8bit[operand2.isRegister] = result;
    }
    return _currentOpcode->cycles;
}

int OpcodeHandler::SET()
{
    RegisterFile& regs = _mem.getsRegs();

    int operand1 = std::stoi(_currentOpcode->operands[0].name);
    operandReturn<uint8_t> operand2 = _mem.get8BitOperand(_currentOpcode->operands[1]);

    uint8_t result = (operand2.value & ~(1 << operand1)) | (1 << operand1);

    if (operand2.isMemoryLocation)
    {
        _mem.getMBC()->write(operand2.address, result);
    }
    else
    {
        *regs.strTo8bit[operand2.isRegister] = result;
    }
    return _currentOpcode->cycles;
}

int OpcodeHandler::RLCA()
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
    return _currentOpcode->cycles;
}

int OpcodeHandler::RRCA()
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
    return _currentOpcode->cycles;
}

int OpcodeHandler::RLA()
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
    return _currentOpcode->cycles;
}

int OpcodeHandler::RRA()
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
    return _currentOpcode->cycles;
}
