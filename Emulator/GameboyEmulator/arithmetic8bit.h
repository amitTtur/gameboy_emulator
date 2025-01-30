#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"

class Arithmetic8bit : public Opcode
{
public:
	Arithmetic8bit(Memory& mem_ref, OpcodeElementHolder* currentOpcode);

	virtual ~Arithmetic8bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:

	void OR();

	void INC();

	void DEC();

	void ADD();

	void SUB();

	void SBC();

	void ADC();

	void AND();

	void XOR();

	void CP();

	// WTF is that istruction
	// see copy source https://github.com/not-chciken/TLMBoy/blob/main/src/cpu_ops.cpp#L12
	void DAA();

	void CPL();

	void SCF();

	void CCF();

	std::map<std::string, void (Arithmetic8bit::*)()> _strToFunc;

};