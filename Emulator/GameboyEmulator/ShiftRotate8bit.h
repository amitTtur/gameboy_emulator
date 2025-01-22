#pragma once
#include "Register.hpp"
#include "Opcode.h"
#include "OpcodeFactory.h"


class ShiftRotate8bit : public Opcode
{
public:
	ShiftRotate8bit(Memory& mem_ref, OpcodeElementHolder& currentOpcode)
		: Opcode(mem_ref), _currentOpcode(currentOpcode)
	{
		// Initialize the map with opcode strings as keys and function pointers as values
		_strToFunc["RLC"] = &ShiftRotate8bit::RLC;
		_strToFunc["RRC"] = &ShiftRotate8bit::RRC;
		_strToFunc["RL"] = &ShiftRotate8bit::RL;
		_strToFunc["RR"] = &ShiftRotate8bit::RR;
		_strToFunc["SLA"] = &ShiftRotate8bit::SLA;
		_strToFunc["SRA"] = &ShiftRotate8bit::SRA;
		_strToFunc["SWAP"] = &ShiftRotate8bit::SWAP;
		_strToFunc["SRL"] = &ShiftRotate8bit::SRL;
		_strToFunc["BIT"] = &ShiftRotate8bit::BIT;
		_strToFunc["RES"] = &ShiftRotate8bit::RES;
		_strToFunc["SET"] = &ShiftRotate8bit::SET;
		_strToFunc["RLCA"] = &ShiftRotate8bit::RLCA;
		_strToFunc["RRCA"] = &ShiftRotate8bit::RRCA;
		_strToFunc["RLA"] = &ShiftRotate8bit::RLA;
		_strToFunc["RRA"] = &ShiftRotate8bit::RRA;
	};

	virtual ~ShiftRotate8bit() = default;

	virtual int run() override;
	virtual void printOpcodeMnemonic() const override;

private:
	// from opcode it has a _mem copy. [Memory& _mem]
	OpcodeElementHolder& _currentOpcode;

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