#pragma once
#include "Opcode.h"
#include "OpcodeFactory.h"

class JumpCallOpcodes : public Opcode
{
public:
	JumpCallOpcodes(Memory& mem_ref, OpcodeElementHolder& currentOpcode) : Opcode(mem_ref), _currentOpcode(currentOpcode) {
		_strToFunc["JR"] = &JumpCallOpcodes::JR;
		_strToFunc["RET"] = &JumpCallOpcodes::RET;
		_strToFunc["RETI"] = &JumpCallOpcodes::RET;
		_strToFunc["JP"] = &JumpCallOpcodes::JP;
		_strToFunc["CALL"] = &JumpCallOpcodes::CALL;
		_strToFunc["RST"] = &JumpCallOpcodes::RST;
	};

	virtual ~JumpCallOpcodes() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:
	// from opcode it has a _mem copy. [Memory& _mem]
	OpcodeElementHolder& _currentOpcode;

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