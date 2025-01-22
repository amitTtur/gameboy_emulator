#include "MiscOpcodes.h"

int MiscOpcodes::run()
{
	auto it = _strToFunc.find(_currentOpcode.mnemonic);

	if (it != _strToFunc.end()) {

		// Call the function pointed to by the iterator
		(this->*(it->second))(); // run the opcode
		return _currentOpcode.cycles;
	}

	throw GeneralException("Unsupported opcode: " + _currentOpcode.mnemonic + ", had appeared at [run-> MiscOpcodes]", UNKNOWN_OPCODE);
}

void MiscOpcodes::printOpcodeMnemonic() const
{
	std::cout << _currentOpcode.mnemonic << " ";
	for (int i = 0; i < _currentOpcode.operands.size(); i++)
	{
		std::cout << _currentOpcode.operands[i].name << " ";
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

}

void MiscOpcodes::HALT()
{
	if (_mem.IME())
	{
		systemHalted = true;
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
