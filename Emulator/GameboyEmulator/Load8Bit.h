#pragma once
#include "Opcode.h"
#include "OpcodeFactory.h"

class Load8Bit : public Opcode
{
public:
	Load8Bit(Memory& mem_ref, OpcodeElementHolder* currentOpcode);

	virtual ~Load8Bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:

	void LD();
};