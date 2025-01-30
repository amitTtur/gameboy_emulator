#include "CPU.h"

CPU::CPU(const std::string& romPath) : _mem(romPath), _opcodeFactory(_mem)
{
	updateMemAndRegsAfterBoot();

	// open the file in appending, and clear it when begining to use it
	//logFile.open("logFile.txt", std::ios::out);
	//if(!logFile.is_open())
	//{
	//	std::cout << "log file didnt open." << std::endl;
	//}
	//writeCurrentStateToLogFile();
}

CPU::~CPU()
{
	logFile.close();
}

int times = 0;

int CPU::ExecuteNextInstruction()
{
	int cycles = 0;
	Opcode* instance = nullptr;

	if (globalVars::systemHalted())
	{
		//std::cout << "halted" << std::endl;
		return 4;
	}

	try
	{
		//instance = _opcodeFactory.getInstance(_mem.getMemoryLocation());
		//cycles = instance->run();
		//delete instance;
		//return cycles;

		return _opcodeFactory.run(_mem.getMemoryLocation());

		/* also for testing, remove after its done
		if (times == 152039)
		{
			std::cout << "here" << std::endl;
		}
		std::cout << "times: " << times 
			<< " IF: " << std::setw(8) << static_cast<int>(_mem.IF()) 
			<< " IE: " << std::setw(8) << static_cast<int>(_mem.IE()) 
			<< (globalVars::systemHalted() ? "  system halted" : " ")
			<< (globalVars::haltBug() ? "  halt bug" : " ") << std::endl;
			
		instance->printOpcodeMnemonic();
		std::cout << std::endl;

		if (times % 100000 == 0) { std::cout << times << std::endl; times++; }

		cycles = instance->run();

		if (!_opcodeFactory.getPrefix() && (!globalVars::systemHalted() || instance->getCurrentOpcode().mnemonic == "HALT"))
		{
			writeCurrentStateToLogFile();
			times++;
		}

		delete instance;*/
	}
	catch (const std::exception& e)
	{
		std::cout << "error: " << e.what() << std::endl;
		return 4;
		//if (instance)
		//{
		//	delete instance;
		//}
	}
	

	//std::cout << "" << _mem.getsRegs().PC << std::endl;

	//return cycles;
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

		//std::cout << "interrupt triggerd" << std::endl;
		globalVars::systemHalted(false);
		_mem.IME(0);

		uint16_t pcAfterCurrentOpcode = regs.PC;

		regs.SP -= 1;
		_mem.getMBC()->write(regs.SP, (pcAfterCurrentOpcode >> 8) & 0xFF); // High byte
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

	if (int_Vblank() && intEnable_Joypad())
	{
		//vblank handle
		performCall(INT_VBLANK_LOC);
		int_Vblank(0);
		return 20;
	}
	if (int_LCDCStatus() && intEnable_LCDCStatus())
	{
		//lcdcstatus handle
		performCall(INT_STAT_LOC);
		int_LCDCStatus(0);
		return 20;
	}
	if (int_timerOverflow() && intEnable_timerOverflow())
	{
		//timer handle
		performCall(INT_TIMER_LOC);
		int_timerOverflow(0);
		return 20;
	}
	if (int_serialTransfer() && intEnable_serialTransfer())
	{
		//serial handle
		performCall(INT_SERIAL_LOC);
		int_serialTransfer(0);
		return 20;
	}
	if (int_Joypad() && intEnable_Joypad())
	{
		//joypad handle
		performCall(INT_JOYPAD_LOC);
		int_Joypad(0);
		return 20;
	}
	return 0;
}


void CPU::updateTimers(uint8_t tCycles)
{
	for (int i = 0; i < tCycles; i++)
	{
		// DIV register update (equivalent to SYSCLK)
		globalVars::DIVRegister(globalVars::DIVRegister() + 1);
		_mem[TIMER_DIV_LOC] = globalVars::DIVRegister() >> 8;

		// Handle TIMA overflow cooldown
		if (globalVars::TIMAOverflowCooldown() > 0)
		{
			globalVars::TIMAOverflowCooldown(globalVars::TIMAOverflowCooldown() - 1);
			if (globalVars::TIMAOverflowCooldown() == 0)
			{
				_mem[TIMER_TIMA_LOC] = _mem[TIMER_TMA_LOC];
				int_timerOverflow(1);
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


void CPU::updateMemAndRegsAfterBoot()
{
	_mem.getsRegs().PC = 0x100;
	_mem.getsRegs().A = 0x1;
	_mem.getsRegs().B = 0x0;
	_mem.getsRegs().C = 0x13;
	_mem.getsRegs().D = 0x0;
	_mem.getsRegs().E = 0xD8;
	_mem.getsRegs().F = 0xB0;
	_mem.getsRegs().H = 0x1;
	_mem.getsRegs().L = 0x4D;
	_mem.getsRegs().SP = 0xFFFE;
	globalVars::DIVRegister(0xabcc);
	globalVars::systemHalted(false);
	_mem.getsRegs().PC = 0;
	_mem.getsRegs().SP = 0xFFFE;
	_fallingEdgeTimerDetection = 0;
}

void CPU::update()
{
	int currentFrameCycles = 0;

	while (currentFrameCycles < CLOCK_CYCLES_PER_FRAME)
	{

		int cycles = 0;// = handleInterrupts();

		cycles += ExecuteNextInstruction();

		currentFrameCycles += cycles;

		updateTimers(cycles);

		//update ppu based on cycles

	}

	int_Vblank(1);
	//ppu render screen
}

uint8_t CPU::int_Vblank() { return _mem.IF() & 1; } //bit 0
uint8_t CPU::int_LCDCStatus() { return (_mem.IF() >> 1) & 1; } //bit 1
uint8_t CPU::int_timerOverflow() { return (_mem.IF() >> 2) & 1; } //bit 2
uint8_t CPU::int_serialTransfer() { return (_mem.IF() >> 3) & 1; } //bit 3
uint8_t CPU::int_Joypad() { return (_mem.IF() >> 4) & 1; } //bit 4

void CPU::int_Vblank(uint8_t val) { _mem.IF() = (_mem.IF() & ~1 | val); } //bit 0
void CPU::int_LCDCStatus(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 1)) | (val << 1); } //bit 1
void CPU::int_timerOverflow(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 2)) | (val << 2); }  //bit 2
void CPU::int_serialTransfer(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 3)) | (val << 3); }  //bit 3
void CPU::int_Joypad(uint8_t val) { _mem.IF() = (_mem.IF() & ~(1 << 4)) | (val << 4); } //bit 4

//interrupts enable
uint8_t CPU::intEnable_Vblank() { return _mem.IE() & 1; } //bit 0
uint8_t CPU::intEnable_LCDCStatus() { return (_mem.IE() >> 1) & 1; } //bit 1
uint8_t CPU::intEnable_timerOverflow() { return (_mem.IE() >> 2) & 1; } //bit 2
uint8_t CPU::intEnable_serialTransfer() { return (_mem.IE() >> 3) & 1; } //bit 3
uint8_t CPU::intEnable_Joypad() { return (_mem.IE() >> 4) & 1; } //bit 4

void CPU::intEnable_Vblank(uint8_t val) { _mem.IE() = (_mem.IE() & ~1 | val); } //bit 0
void CPU::intEnable_LCDCStatus(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 1)) | (val << 1); } //bit 1
void CPU::intEnable_timerOverflow(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 2)) | (val << 2); }  //bit 2
void CPU::intEnable_serialTransfer(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 3)) | (val << 3); }  //bit 3
void CPU::intEnable_Joypad(uint8_t val) { _mem.IE() = (_mem.IE() & ~(1 << 4)) | (val << 4); } //bit 4
