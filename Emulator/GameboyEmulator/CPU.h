#include <cstdint>

#include "Register.hpp"
#include "Memory.h"
#include "OpcodeFactory.h"
#include "CodesAndDefines.h"
#include "globalVars.h"

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

	//interrupts enable
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
