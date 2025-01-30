#include "OpcodeFactory.h"

OpcodeFactory::OpcodeFactory(Memory& memoryReferance) : _memRef(memoryReferance),
    _arithmetic8bit(new Arithmetic8bit(memoryReferance, &_currentOpcode)),
    _arithmetic16bit(new Arithmetic16bit(memoryReferance, &_currentOpcode)),
    _jumpCallOpcodes(new JumpCallOpcodes(memoryReferance, &_currentOpcode)),
    _load8bit(new Load8Bit(memoryReferance, &_currentOpcode)),
    _load16bit(new Load16Bit(memoryReferance, &_currentOpcode)),
    _miscOpcodes(new MiscOpcodes(memoryReferance, &_currentOpcode, *this)),
    _shiftRotate8bit(new ShiftRotate8bit(memoryReferance, &_currentOpcode))
{

    initializeOpcodeMap();
    _prefix = false;
}

int OpcodeFactory::run(uint8_t opcode)
{
    if (_opcodesMap.find(opcode) == _opcodesMap.end() || _cbOpcodesMap.find(opcode) == _cbOpcodesMap.end())
    {
        throw GeneralException("no opcode with that number exists in the opcode map... [GetInstance]", UNKNOWN_OPCODE);
    }

    _currentOpcode = _prefix ? _cbOpcodesMap[opcode] : _opcodesMap[opcode];
  
    // case prefix is called
    if (_prefix || isUniqueShifters(_currentOpcode.mnemonic))
    {
        // l operation in cb prefix are shifters no need to check
        _prefix = false;
        return _shiftRotate8bit->run();
    }
    

    /****** From here is the unprefixed (regular) opcodes table ******/

    if (isMisc(_currentOpcode.mnemonic))
    {
        // ALTHOUGH IT IS A POINTER, DONT DELETE THE INSTANCE OF FACTORY
        return _miscOpcodes->run();
    }

    if (isArithmetic(_currentOpcode.mnemonic))
    {
        if (areOperands16BitsLD(_currentOpcode))
        {
            return _arithmetic16bit->run();
        }
        return _arithmetic8bit->run();
    }

    if (isLoad(_currentOpcode.mnemonic))
    {
        if (areOperands16BitsLD(_currentOpcode))
        {
            return _load16bit->run();
        }
        return _load8bit->run();
    }

    if (isJump(_currentOpcode.mnemonic))
    {
        return _jumpCallOpcodes->run();
    }

    // probably an unregistered opcode in that case gb (the real one) would crash.
    throw GeneralException("Opcode with an unkown type??? [GetInstance]", UNKNOWN_OPCODE);

}

void OpcodeFactory::setPrefix(const bool& val)
{
    _prefix = val;
}

bool OpcodeFactory::getPrefix() const
{
    return _prefix;
}


void OpcodeFactory::initializeOpcodeMap()
{
    json j = getOpcodesJsonContents();

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
        _opcodesMap.insert(std::pair<int, OpcodeElementHolder>(std::stoi(item.key(), nullptr, 16), tmp));
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
        _cbOpcodesMap.insert(std::pair<int, OpcodeElementHolder>(std::stoi(item.key(), nullptr, 16), tmp));
    }
}

json OpcodeFactory::getOpcodesJsonContents()
{
    std::ifstream file("opcodes.json");

    if (!file.is_open()) {
        throw GeneralException("opcode factory initialization couldn't open the opcodes.json file...", FILE_WONT_OPEN);
    }

    json j;
    file >> j;
    return j;
}

bool OpcodeFactory::isArithmetic(const std::string& mnemonic) const
{
    return _arithmeticNames.find(mnemonic) != _arithmeticNames.end();
}

inline bool OpcodeFactory::isLoad(const std::string& mnemonic) const
{
    return _loadNames.find(mnemonic) != _loadNames.end();
}

inline bool OpcodeFactory::isShift(const std::string& mnemonic) const
{
    return _shiftNames.find(mnemonic) != _shiftNames.end();
}

inline bool OpcodeFactory::isMisc(const std::string& mnemonic) const
{
    return _miscNames.find(mnemonic) != _miscNames.end();
}

inline bool OpcodeFactory::isJump(const std::string& mnemonic) const
{
    return _jumpNames.find(mnemonic) != _jumpNames.end();
}

inline bool OpcodeFactory::isUniqueShifters(const std::string& mnemonic) const
{
    return  _uniqueShftersNames.find(mnemonic) != _uniqueShftersNames.end();
}

inline bool OpcodeFactory::areOperands16BitsLD(const OpcodeElementHolder& opcode)
{
    for (const operandElementHolder& operand : opcode.operands)
    {
        if (operand.immediate)
        {
            if (operand.name == "BC" || operand.name == "AF" || operand.name == "DE" || operand.name == "HL" || operand.name == "SP" || operand.name == "n16") { return true; }
        }
    }

    return false;
}
