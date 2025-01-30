#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"


class Arithmetic16bit : public Opcode
{
public:
	Arithmetic16bit(Memory& mem_ref, OpcodeElementHolder* currentOpcode);

	virtual ~Arithmetic16bit() = default;

	virtual int run() override;

	virtual void printOpcodeMnemonic() const override;

private:


	void INC();

	void DEC();

	void ADD();

	std::map<std::string, void (Arithmetic16bit::*)()> _strToFunc;

};