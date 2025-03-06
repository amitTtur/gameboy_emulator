#include <cstdint>

#include "Register.hpp"
#include "Memory.h"
#include "OpcodeHandler.h"
#include "CodesAndDefines.h"
#include "globalVars.h"
#include "PPU.h"

#ifndef CPU_H
#define CPU_H


class CPU 
{
public:
	// need to have a rom path
	CPU() = delete;

	CPU(PPU& ppu, Memory& mem);
	~CPU();
	
	void update();



private:
	
	Memory& _mem;
	OpcodeHandler _opcodeHandler;
	PPU& _ppu;
	uint8_t _fallingEdgeTimerDetection; //used to keep the calculation of the TIMA increase between frames


	void updateMemAndRegsAfterBoot();
	int ExecuteNextInstruction();
	int handleInterrupts();
	void updateTimers(uint8_t tCycles);

	// for tests
	std::ofstream logFile;
	void writeCurrentStateToLogFile();

	bool interruptPending();

	void doDma();
};

#endif // !CPU_H
