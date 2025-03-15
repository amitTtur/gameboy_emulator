#include "CPU.h"

CPU::CPU(PPU& ppu, Memory& mem) : _mem(mem), _opcodeHandler(_mem), _ppu(ppu)
{
	updateMemAndRegsAfterBoot();
}

CPU::~CPU()
{
	logFile.close();
}


int CPU::ExecuteNextInstruction()
{
	int cycles = 0;

	try
	{
		if (!_opcodeHandler.getPrefix())
		{
			cycles = handleInterrupts();
			//DO NOT HANDLE INTERRUPTS AFTER PREFIX	
		}
		if (globalVars::systemHalted())
		{
			return 4;
		}
		if (cycles > 0)
		{
			return cycles;
		}

		if (globalVars::dma())
		{
			doDma();
			globalVars::dma(false);
			cycles = 4;
		}

		cycles += _opcodeHandler.run(_mem.getMemoryLocation());

		return cycles;
	}
	catch (const std::exception& e)
	{
		std::cout << "error: " << e.what() << std::endl;
		return 4;
	}
	
}

int CPU::handleInterrupts()
{
	//#define INT_VBLANK_LOC 0x40
	//#define INT_STAT_LOC 0x48
	//#define INT_TIMER_LOC 0x50
	//#define INT_SERIAL_LOC 0x58
	//#define INT_JOYPAD_LOC 0x60

	RegisterFile& regs = _mem.getsRegs();

	auto performCall = [&](uint8_t n) 
	{

		globalVars::systemHalted(false);
		_mem.IME(0);

		uint16_t pcAfterCurrentOpcode = regs.PC;

		regs.SP -= 1;
		_mem.getMBC()->write(regs.SP, (pcAfterCurrentOpcode >> 8) & 0xFF);
		regs.SP -= 1;
		_mem.getMBC()->write(regs.SP, pcAfterCurrentOpcode & 0xFF);

		regs.PC = n;
	};

	if (!_mem.IME())
	{
		if (globalVars::systemHalted() && ((_mem.IF() & _mem.IE() & 0x1f) != 0))
		{
			globalVars::systemHalted(false);
		}

		return 0;
	}

	if (_mem.int_Vblank() & _mem.intEnable_Vblank())
	{
		//vblank handle
		performCall(INT_VBLANK_LOC);
		_mem.int_Vblank(0);
		return 20;
	}
	if (_mem.int_LCDCStatus() & _mem.intEnable_LCDCStatus())
	{
		//lcdcstatus handle
		performCall(INT_STAT_LOC);
		_mem.int_LCDCStatus(0);
		return 20;
	}
	if (_mem.int_timerOverflow() & _mem.intEnable_timerOverflow())
	{
		//timer handle
		performCall(INT_TIMER_LOC);
		_mem.int_timerOverflow(0);
		return 20;
	}
	if (_mem.int_serialTransfer() & _mem.intEnable_serialTransfer())
	{
		//serial handle
		performCall(INT_SERIAL_LOC);
		_mem.int_serialTransfer(0);
		return 20;
	}
	if (_mem.int_Joypad() & _mem.intEnable_Joypad())
	{
		//joypad handle
		performCall(INT_JOYPAD_LOC);
		_mem.int_Joypad(0);
		return 20;
	}
	return 0;
}


void CPU::updateTimers(uint8_t tCycles)
{
	for (int i = 0; i < tCycles; i++)
	{
		globalVars::clock++;
		if (globalVars::clock >= CLOCK_CYCLES_PER_SECOND)
		{
			globalVars::clock %= CLOCKS_PER_SEC;
			if (globalVars::canIncreaseMbc3Timer())
			{
				globalVars::setMBC3Timer(globalVars::getMBC3Timer() + 1);
			}
		}

		// DIV register update (equivalent to SYSCLK)
		globalVars::DIVRegister(globalVars::DIVRegister() + 1);
		_mem[TIMER_DIV_LOC] = uint8_t(globalVars::DIVRegister() >> 8);

		// Handle TIMA overflow cooldown
		if (globalVars::TIMAOverflowCooldown() > 0)
		{
			globalVars::TIMAOverflowCooldown(globalVars::TIMAOverflowCooldown() - 1);
			if (globalVars::TIMAOverflowCooldown() == 0)
			{
				_mem[TIMER_TIMA_LOC] = _mem[TIMER_TMA_LOC];
				_mem.int_timerOverflow(1);
			}
			continue;
		}

		// Determine which bit to use based on TAC
		uint8_t divBit;
		switch (_mem[TIMER_TAC_LOC] & 0b11)
		{
		case 0b00: // bit 9
			divBit = (globalVars::DIVRegister() >> 9) & 1;
			break;
		case 0b01: // bit 3
			divBit = (globalVars::DIVRegister() >> 3) & 1;
			break;
		case 0b10: // bit 5
			divBit = (globalVars::DIVRegister() >> 5) & 1;
			break;
		case 0b11: // bit 7
			divBit = (globalVars::DIVRegister() >> 7) & 1;
			break;
		}

		// Check if timer is enabled
		uint8_t timerEnabled = (_mem[TIMER_TAC_LOC] >> 2) & 1;
		uint8_t andResult = divBit & timerEnabled;

		if (_fallingEdgeTimerDetection == 1 && andResult == 0)
		{
			// Increment TIMA
			if (_mem[TIMER_TIMA_LOC] == 0xFF)
			{
				// Overflow condition
				globalVars::TIMAOverflowCooldown(3);
				_mem[TIMER_TIMA_LOC] = 0;
			}
			else
			{
				_mem[TIMER_TIMA_LOC] = _mem[TIMER_TIMA_LOC] + 1;
			}
		}
		_fallingEdgeTimerDetection = andResult;
	}
}


void CPU::writeCurrentStateToLogFile()
{
	// no ref needed
	RegisterFile& regs = _mem.getsRegs();
	MBC* mbc = _mem.getMBC();
	logFile << std::hex << std::setfill('0') // Set output to hex and fill with zeros
		<< "A:" << std::setw(2) << static_cast<int>(regs.A) // Assuming regs.A is uint8_t
		<< " F:" << std::setw(2) << static_cast<int>(regs.F)
		<< " B:" << std::setw(2) << static_cast<int>(regs.B)
		<< " C:" << std::setw(2) << static_cast<int>(regs.C)
		<< " D:" << std::setw(2) << static_cast<int>(regs.D)
		<< " E:" << std::setw(2) << static_cast<int>(regs.E)
		<< " H:" << std::setw(2) << static_cast<int>(regs.H)
		<< " L:" << std::setw(2) << static_cast<int>(regs.L)
		<< " SP:" << std::setw(4) << regs.SP // Assuming regs.SP is uint16_t
		<< " PC:" << std::setw(4) << regs.PC // Assuming regs.PC is uint16_t
		<< " PCMEM:" << std::setw(2) << static_cast<int>(mbc->read(regs.PC))
		<< "," << std::setw(2) << static_cast<int>(mbc->read(regs.PC + 1))
		<< "," << std::setw(2) << static_cast<int>(mbc->read(regs.PC + 2))
		<< "," << std::setw(2) << static_cast<int>(mbc->read(regs.PC + 3))
		<< std::endl;
}

bool CPU::interruptPending()
{
	return _mem.int_Vblank() || _mem.int_LCDCStatus() || _mem.int_timerOverflow() || _mem.int_serialTransfer() || _mem.int_Joypad();
}

void CPU::doDma()
{
	// dma goes through 160 (0xA0) memory locations
	uint16_t offset = _mem[DMA_LOC] << 8;
	for (int i = 0; i < 0xA0; i++)
	{
		_mem[OAM_MEMORY_START + i] = _mem[offset + i];
	}
}


void CPU::updateMemAndRegsAfterBoot()
{
	_fallingEdgeTimerDetection = 0;

	_mem.getsRegs().PC = 0x100;
	_mem.getsRegs().A = 0x1;
	_mem.getsRegs().B = 0x0;
	_mem.getsRegs().C = 0x13;
	_mem.getsRegs().D = 0x0;
	_mem.getsRegs().E = 0xD8;
	_mem.getsRegs().F = 0;
	_mem.getsRegs().ZF(true);
	_mem.getsRegs().H = 0x1;
	_mem.getsRegs().L = 0x4D;
	_mem.getsRegs().SP = 0xFFFE;

	_mem[TIMER_TAC_LOC] = 0xf8;
	_mem[INPUT_REG_LOC] = 0xfc;
	_mem[INTERRUPT_ENABLE_LOC] = 0;
	_mem[INTERRUPT_FLAG_LOC] = 0xe1;
	_mem[BG_PLATTE_LOC] = 0xfc;
	_mem[LY_LOC] = 0x91;
	_mem[LYC_LOC] = 0;
	_mem[SCX_LOC] = 0;
	_mem[SCY_LOC] = 0;
	_mem[WX_LOC] = 0;
	_mem[WY_LOC] = 0;


	globalVars::DIVRegister(0xabcc);
	globalVars::systemHalted(false);
	globalVars::dma(false);
	globalVars::padState(0);

}

void CPU::update()
{
	int currentFrameCycles = 0;

	while (currentFrameCycles < CLOCK_CYCLES_PER_FRAME)
	{
		int cycles = ExecuteNextInstruction();

		currentFrameCycles += cycles;

		updateTimers(cycles);

		_ppu.update(cycles);

	}
}

