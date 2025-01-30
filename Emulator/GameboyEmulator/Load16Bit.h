#pragma once
#include "Opcode.h"
#include "OpcodeFactory.h"

class Load16Bit : public Opcode
{
public:
	Load16Bit(Memory& mem_ref, OpcodeElementHolder* currentOpcode);

	virtual ~Load16Bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;
private:

	void LD();

	void POP();

	void PUSH();

	void specialLD();

	std::map<std::string, void (Load16Bit::*)()> _strToFunc;

};