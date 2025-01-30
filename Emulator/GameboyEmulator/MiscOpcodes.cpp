#include "MiscOpcodes.h"

MiscOpcodes::MiscOpcodes(Memory& mem_ref, OpcodeElementHolder* currentOpcode, OpcodeFactory& factory)
	: Opcode(mem_ref, currentOpcode), _factory(factory) // , _factory(factory_ref)
{
	// Initialize the map with opcode strings as keys and function pointers as values
	_strToFunc["NOP"] = &MiscOpcodes::NOP;
	_strToFunc["STOP"] = &MiscOpcodes::STOP;
	_strToFunc["HALT"] = &MiscOpcodes::HALT;
	_strToFunc["PREFIX"] = &MiscOpcodes::PREFIX;
	_strToFunc["DI"] = &MiscOpcodes::DI;
	_strToFunc["EI"] = &MiscOpcodes::EI;
}

int MiscOpcodes::run()
{
	auto it = _strToFunc.find(_currentOpcode->mnemonic);

	if (it != _strToFunc.end()) {

		// Call the function pointed to by the iterator
		(this->*(it->second))(); // run the opcode
		return _currentOpcode->cycles;
	}

	throw GeneralException("Unsupported opcode: " + _currentOpcode->mnemonic + ", had appeared at [run-> MiscOpcodes]", UNKNOWN_OPCODE);
}

void MiscOpcodes::printOpcodeMnemonic() const
{
	std::cout << _currentOpcode->mnemonic << " ";
	for (int i = 0; i < _currentOpcode->operands.size(); i++)
	{
		std::cout << _currentOpcode->operands[i].name << " ";
	}
	std::cout << std::endl;
}

void MiscOpcodes::NOP()
{
	//this is the fun space
	// whoooooooo fun spaceeeeeeeeeeee
}

void MiscOpcodes::STOP()
{
	globalVars::running(false);
}

void MiscOpcodes::HALT()
{
	if (_mem.IME() == 0 && (_mem.IE() & _mem.IF() & 0x1f) != 0)
	{
		globalVars::haltBug(true);
	}
	else
	{
		globalVars::systemHalted(true);
	}
}

void MiscOpcodes::PREFIX()
{
	_factory.setPrefix(true);
}

void MiscOpcodes::DI()
{
	_mem.IME(0);
}

void MiscOpcodes::EI()
{
	_mem.IME(1);
}
