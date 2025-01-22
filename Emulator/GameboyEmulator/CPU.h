#include <cstdint>

#include "Register.hpp"
#include "Memory.h"
#include "OpcodeFactory.h"
#include "CodesAndDefines.h"

#ifndef CPU_H
#define CPU_H


class CPU 
{
public:
	// need to have a rom path
	CPU() = delete;

	CPU(const std::string& romPath);
	~CPU();
	
	void update();


	//interrupts set
	uint8_t int_Vblank() { return _mem.IF() & 1; }; //bit 0
	uint8_t int_LCDCStatus() { return (_mem.IF() >> 1) & 1; }; //bit 1
	uint8_t int_timerOverflow() { return (_mem.IF() >> 2) & 1; }; //bit 2
	uint8_t int_serialTransfer() { return (_mem.IF() >> 3) & 1; }; //bit 3
	uint8_t int_Joypad() { return (_mem.IF() >> 4) & 1; }; //bit 4

	void int_Vblank(uint8_t val) { _mem.IF() = (_mem.IF() & ~1 | val); } //bit 0
	void int_LCDCStatus(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 1)) | (val << 1); }; //bit 1
	void int_timerOverflow(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 2)) | (val << 2); };  //bit 2
	void int_serialTransfer(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 3)) | (val << 3); };  //bit 3
	void int_Joypad(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 4)) | (val << 4); }; //bit 4

	//interrupts enable
	uint8_t intEnable_Vblank() { return _mem.IE() & 1; }; //bit 0
	uint8_t intEnable_LCDCStatus() { return (_mem.IE() >> 1) & 1; }; //bit 1
	uint8_t intEnable_timerOverflow() { return (_mem.IE() >> 2) & 1; }; //bit 2
	uint8_t intEnable_serialTransfer() { return (_mem.IE() >> 3) & 1; }; //bit 3
	uint8_t intEnable_Joypad() { return (_mem.IE() >> 4) & 1; }; //bit 4

	void intEnable_Vblank(uint8_t val) { _mem.IE() = (_mem.IE() & ~1 | val); } //bit 0
	void intEnable_LCDCStatus(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 1)) | (val << 1); }; //bit 1
	void intEnable_timerOverflow(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 2)) | (val << 2); };  //bit 2
	void intEnable_serialTransfer(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 3)) | (val << 3); };  //bit 3
	void intEnable_Joypad(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 4)) | (val << 4); }; //bit 4

private:
	Memory _mem;
	OpcodeFactory _opcodeFactory;

	uint8_t _fallingEdgeTimerDetection; //used to keep the calculation of the TIMA increase between frames

	void updateMemAndRegsAfterBoot();
	int ExecuteNextInstruction();
	int handleInterrupts();
	void updateTimers(uint8_t tCycles);

	// for tests
	std::ofstream logFile;
	void writeCurrentStateToLogFile();
};

#endif // !CPU_H
