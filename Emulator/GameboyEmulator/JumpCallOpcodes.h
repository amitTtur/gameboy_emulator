#pragma once
#include "Opcode.h"
#include "OpcodeFactory.h"

class JumpCallOpcodes : public Opcode
{
public:
	JumpCallOpcodes(Memory& mem_ref, OpcodeElementHolder* currentOpcode);

	virtual ~JumpCallOpcodes() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:

	int JR();

	int RET(); // incldues RETI in it

	int JP();

	int CALL();

	// rst is an instruction the jumps to the given memory space 
	// example rst 3, jums to the 3rd byte of the memory (into the bios code) 
	// so rst pushes the current pc location into the stack, and changes the pc value
	// to 0x18 of the memory location
	int RST();

	std::map<std::string, int (JumpCallOpcodes::*)()> _strToFunc;

};