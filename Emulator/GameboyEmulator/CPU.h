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
	CPU() = delete;

	CPU(PPU& ppu, Memory& mem);
	~CPU();
	
	//one frame update
	void update();

private:
	
	Memory& _mem;
	OpcodeHandler _opcodeHandler;
	PPU& _ppu;

	//used to keep the calculation of the TIMA increase between frames
	uint8_t _fallingEdgeTimerDetection;

	//bootup
	void updateMemAndRegsAfterBoot();

	//next cpu instruction
	int ExecuteNextInstruction();

	//interrupts
	int handleInterrupts();
	bool interruptPending();

	//timers
	void updateTimers(uint8_t tCycles);

	//fast vram transfer
	void doDma();

	//debugging
	std::ofstream logFile;
	void writeCurrentStateToLogFile();


};

#endif // !CPU_H
