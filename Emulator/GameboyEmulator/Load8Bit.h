#pragma once
#include "Opcode.h"
#include "OpcodeFactory.h"

class Load8Bit : public Opcode
{
public:
	Load8Bit(Memory& mem_ref, OpcodeElementHolder& currentOpcode) : Opcode(mem_ref), _currentOpcode(currentOpcode) {};

	virtual ~Load8Bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:
	// from opcode it has a _mem copy. [Memory& _mem]
	OpcodeElementHolder& _currentOpcode;

	void LD();
};