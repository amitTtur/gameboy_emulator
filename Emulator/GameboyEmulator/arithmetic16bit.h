#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"


class Arithmetic16bit : public Opcode
{
public:
	Arithmetic16bit(Memory& mem_ref, OpcodeElementHolder& currentOpcode)
		: Opcode(mem_ref), _currentOpcode(currentOpcode)
	{
		// Initialize the map with opcode strings as keys and function pointers as values
		_strToFunc["ADD"] = &Arithmetic16bit::ADD;
		_strToFunc["INC"] = &Arithmetic16bit::INC;
		_strToFunc["DEC"] = &Arithmetic16bit::DEC;
	};

	virtual ~Arithmetic16bit() = default;

	virtual int run() override;

	virtual void printOpcodeMnemonic() const override;

private:
	// from opcode it has a _mem copy. [Memory& _mem]
	OpcodeElementHolder& _currentOpcode;

	void INC();

	void DEC();

	void ADD();

	std::map<std::string, void (Arithmetic16bit::*)()> _strToFunc;

};