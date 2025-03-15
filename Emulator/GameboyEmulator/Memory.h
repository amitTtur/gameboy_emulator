#pragma once
#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>

#include "dataStructures.hpp"
#include "GeneralException.h"
#include "Register.hpp"

#include "MBC1.h"
#include "MBC2.h"
#include "MBC3.h"
#include "NoMBC.h"
#include "MBC5.h"


class Memory {
public:
	Memory();
	~Memory();

	//initiate memory and mbc
	void init(const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag);

	//returns reference to memory at index
	uint8_t& operator[](const uint16_t& index);

	RegisterFile& getsRegs();

	uint8_t getMemoryLocation(); // returns memory at pc
	uint16_t getMemoryLocation16bit(); // return memory at pc (16 bit)
	uint8_t getMemoryLocation(const uint16_t& index); // returns memory at index
	uint16_t getMemoryLocation16bit(const uint16_t& index);// returns memory at index (16 bit)

	operandReturn<uint8_t> get8BitOperand(const operandElementHolder& operand); //returns operand for opcodes
	operandReturn<uint8_t> get8BitOperand(const std::string operand); //returns operand for opcodes
	operandReturn<uint16_t> get16BitOperand(const operandElementHolder& operand); //returns operand for opcodes (16 bit)

	//interrupt master enable
	uint8_t IME() { return _IME; };
	void IME(uint8_t v) { _IME = v; };

	//interrupt flag and interrupt enable flag
	Register<uint8_t>& IE() { return _IE; };
	Register<uint8_t>& IF() { return _IF; };

	//interrupts set
	uint8_t int_Vblank();
	uint8_t int_LCDCStatus();
	uint8_t int_timerOverflow();
	uint8_t int_serialTransfer();
	uint8_t int_Joypad();

	void int_Vblank(uint8_t val);
	void int_LCDCStatus(uint8_t val);
	void int_timerOverflow(uint8_t val);
	void int_serialTransfer(uint8_t val);
	void int_Joypad(uint8_t val);

	//interrupts enable set
	uint8_t intEnable_Vblank();
	uint8_t intEnable_LCDCStatus();
	uint8_t intEnable_timerOverflow();
	uint8_t intEnable_serialTransfer();
	uint8_t intEnable_Joypad();

	void intEnable_Vblank(uint8_t val);
	void intEnable_LCDCStatus(uint8_t val);
	void intEnable_timerOverflow(uint8_t val);
	void intEnable_serialTransfer(uint8_t val);
	void intEnable_Joypad(uint8_t val);

	MBC* getMBC();
private:
	//memory
	uint8_t _mem[0x10000];
	//registers
	RegisterFile _regs;
	//interrupts
	Register<uint8_t> _IE;
	Register<uint8_t> _IF;
	uint8_t _IME;
	//mbc
	MBC* _mbc;
};

#endif
