#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"

class Arithmetic8bit : public Opcode
{
public:
	Arithmetic8bit(Memory& mem_ref, OpcodeElementHolder& currentOpcode)
		: Opcode(mem_ref), _currentOpcode(currentOpcode)
	{
		// Initialize the map with opcode strings as keys and function pointers as values
		_strToFunc["ADD"] = &Arithmetic8bit::ADD;
		_strToFunc["SUB"] = &Arithmetic8bit::SUB;
		_strToFunc["AND"] = &Arithmetic8bit::AND;
		_strToFunc["OR"] = &Arithmetic8bit::OR;
		_strToFunc["INC"] = &Arithmetic8bit::INC;
		_strToFunc["DEC"] = &Arithmetic8bit::DEC;
		_strToFunc["SBC"] = &Arithmetic8bit::SBC;
		_strToFunc["ADC"] = &Arithmetic8bit::ADC;
		_strToFunc["XOR"] = &Arithmetic8bit::XOR;
		_strToFunc["CP"] = &Arithmetic8bit::CP;
		_strToFunc["DAA"] = &Arithmetic8bit::DAA;
		_strToFunc["CPL"] = &Arithmetic8bit::CPL;
		_strToFunc["SCF"] = &Arithmetic8bit::SCF;
		_strToFunc["CCF"] = &Arithmetic8bit::CCF;
	};

	virtual ~Arithmetic8bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:
	// from opcode it has a _mem copy. [Memory& _mem]
	OpcodeElementHolder& _currentOpcode;

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