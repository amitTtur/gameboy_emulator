#pragma once
#include "Opcode.h"
#include "OpcodeFactory.h"

class Load16Bit : public Opcode
{
public:
	Load16Bit(Memory& mem_ref, OpcodeElementHolder& currentOpcode) : Opcode(mem_ref), _currentOpcode(currentOpcode) {
		_strToFunc["LD"] = &Load16Bit::LD;
		_strToFunc["POP"] = &Load16Bit::POP;
		_strToFunc["PUSH"] = &Load16Bit::PUSH;
	};

	virtual ~Load16Bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;
private:
	// from opcode it has a _mem copy. [Memory& _mem]
	OpcodeElementHolder& _currentOpcode;

	void LD();

	void POP();

	void PUSH();

	void specialLD();

	std::map<std::string, void (Load16Bit::*)()> _strToFunc;

};