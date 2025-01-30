#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"


class ShiftRotate8bit : public Opcode
{
public:
	ShiftRotate8bit(Memory& mem_ref, OpcodeElementHolder* currentOpcode);

	virtual ~ShiftRotate8bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:

	void RLC();

	void RRC();

	void RL();

	void RR();

	void SLA();

	void SRA();

	void SWAP();

	void SRL();

	void BIT();

	void RES();

	void SET();

	void RLCA();

	void RRCA();

	void RLA();

	void RRA();

	std::map<std::string, void (ShiftRotate8bit::*)()> _strToFunc;

};