#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"

class OpcodeFactory;

class MiscOpcodes : public Opcode
{
public:
	MiscOpcodes(Memory& mem_ref, OpcodeElementHolder* currentOpcode, OpcodeFactory& factory);

	// so factory pointer wont get deleted accidentaly.
	virtual ~MiscOpcodes() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:

	OpcodeFactory& _factory;

	void NOP();

	void STOP();

	void HALT();

	void PREFIX();

	void DI();

	void EI();

	std::map<std::string, void (MiscOpcodes::*)()> _strToFunc;

};