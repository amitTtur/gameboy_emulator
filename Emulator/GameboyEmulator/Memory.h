#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include "dataStructures.hpp"
#include "GeneralException.h"
#include "Register.hpp"
#include "MBC1.h"
#include "NoMBC.h"

#include <cstdint>

class Memory {
public:
	Memory(const std::string& romPath);
	~Memory();

	uint8_t& operator[](const uint16_t& index);

	RegisterFile& getsRegs();

	// ALL SHOULD TAKE 4 CYCLES TO FETCH BEWARE AFTER TIMING REASEARCH
	// BEWARE IT RETURNS AN REFERANCE, dont change its value if not requested to do so
	uint8_t getMemoryLocation();  // reads at pc and returns the index as well as raising pc by 1 and 2 respectivly
	uint16_t getMemoryLocation16bit(); // // is ref to the tmpvalue, changing it wont affect anything
	uint8_t getMemoryLocation(const uint16_t& index); // is a referance
	uint16_t getMemoryLocation16bit(const uint16_t& index);// isn't a referance doesnt need to be a referance

	operandReturn<uint8_t> get8BitOperand(const operandElementHolder& operand);
	operandReturn<uint8_t> get8BitOperand(const std::string operand);
	operandReturn<uint16_t> get16BitOperand(const operandElementHolder& operand);

	uint8_t IME() { return _IME; };
	void IME(uint8_t v) { _IME = v; };

	Register<uint8_t>& IE() { return _IE; };
	Register<uint8_t>& IF() { return _IF; };

	MBC* getMBC();
private:
	uint8_t _mem[0x10000];
	RegisterFile _regs;
	Register<uint8_t> _IE;
	Register<uint8_t> _IF;

	uint8_t _IME; //interrupts master enable

	MBC* _mbc;
};

#endif // !MEMORY_H
