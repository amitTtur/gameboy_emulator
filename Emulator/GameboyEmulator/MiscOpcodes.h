#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"

class OpcodeFactory;

class MiscOpcodes : public Opcode
{
public:
	MiscOpcodes(Memory& mem_ref, OpcodeElementHolder& currentOpcode, OpcodeFactory& factory) //, OpcodeFactory* factory_ref)
		: Opcode(mem_ref), _currentOpcode(currentOpcode), _factory(factory) // , _factory(factory_ref)
	{
		// Initialize the map with opcode strings as keys and function pointers as values
		_strToFunc["NOP"] = &MiscOpcodes::NOP;
		_strToFunc["STOP"] = &MiscOpcodes::STOP;
		_strToFunc["HALT"] = &MiscOpcodes::HALT;
		_strToFunc["PREFIX"] = &MiscOpcodes::PREFIX;
		_strToFunc["DI"] = &MiscOpcodes::DI;
		_strToFunc["EI"] = &MiscOpcodes::EI;
	};

	// so factory pointer wont get deleted accidentaly.
	virtual ~MiscOpcodes() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:
	// from opcode it has a _mem copy. [Memory& _mem]
	OpcodeElementHolder& _currentOpcode;
	OpcodeFactory& _factory;

	void NOP();

	void STOP();

	void HALT();

	void PREFIX();

	void DI();

	void EI();

	std::map<std::string, void (MiscOpcodes::*)()> _strToFunc;

};