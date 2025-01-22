#include "CPU.h"

CPU::CPU(const std::string& romPath) : _mem(romPath), _opcodeFactory(_mem)
{
	systemHalted = false;
	_mem.getsRegs().PC = 0;
	_mem.getsRegs().SP = 0xFFFE;
	_fallingEdgeTimerDetection = 0; //maybe start at 1, need to check
	// add the boot code here future amit
	updateMemAndRegsAfterBoot();

	// open the file in appending, and clear it when begining to use it
	logFile.open("logFile.txt", std::ios::out);
	if(!logFile.is_open())
	{
		std::cout << "log file didnt open." << std::endl;
	}
	writeCurrentStateToLogFile();
}

CPU::~CPU()
{
	logFile.close();
}

int times = 0;

int CPU::ExecuteNextInstruction()
{
	int cycles = 1;
	Opcode* instance = nullptr;

	if (systemHalted)
	{
		std::cout << "halted" << std::endl;
		return 4;
	}


	//do factory shit
	try
	{
		// also for testing, remove after its done
		if (times == 152037)
		{
			std::cout << "here" << std::endl;
		}

		
		instance = _opcodeFactory.getInstance(_mem.getMemoryLocation());

		//instance->printOpcodeMnemonic();

		if (times % 100000 == 0) { std::cout << times << std::endl; times++; }

		cycles = instance->run();

		delete instance;
	}
	catch (const std::exception& e)
	{
		std::cout << "error: " << e.what() << std::endl;
		if (instance)
		{
			delete instance;
		}
	}
	

	//std::cout << "" << _mem.getsRegs().PC << std::endl;

	return cycles; //return ammount of clock cycles used
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

		std::cout << "interrupt triggerd" << std::endl;
		systemHalted = false;

		uint16_t pcAfterCurrentOpcode = regs.PC;

		regs.SP -= 1;
		_mem.getMBC()->write(regs.SP, (pcAfterCurrentOpcode >> 8) & 0xFF); // High byte
		regs.SP -= 1;
		_mem.getMBC()->write(regs.SP, pcAfterCurrentOpcode & 0xFF);

		regs.PC = n;
		//regs.PC = (_mem[n] << 8) | _mem[n + 1];
	};

	if (int_Vblank() && intEnable_Joypad())
	{
		//vblank handle

		//push pc to stack
		//put INT_VBLANK_LOC in pc
		_mem.IME(0);
		performCall(INT_VBLANK_LOC);
		int_Vblank(0);
		return 20;
	}
	if (int_LCDCStatus() && intEnable_LCDCStatus())
	{
		//lcdcstatus handle

		//push pc to stack
		//put INT_STAT_LOC in pc
		_mem.IME(0);
		performCall(INT_STAT_LOC);
		int_LCDCStatus(0);
		return 20;
	}
	if (int_timerOverflow() && intEnable_timerOverflow())
	{
		//timer handle

		//push pc to stack
		//put INT_TIMER_LOC in pc
		_mem.IME(0);
		performCall(INT_TIMER_LOC);
		int_timerOverflow(0);
		return 20;
	}
	if (int_serialTransfer() && intEnable_serialTransfer())
	{
		//serial handle
		_mem.IME(0);
		performCall(INT_SERIAL_LOC);
		int_serialTransfer(0);
		return 20;
	}
	if (int_Joypad() && intEnable_Joypad())
	{
		//joypad handle

		//push pc to stack
		//put INT_JOYPAD_LOC in pc
		_mem.IME(0);
		performCall(INT_JOYPAD_LOC);
		int_Joypad(0);
		return 20;
	}
	return 0;
}

void CPU::updateTimers(uint8_t tCycles)
{
	uint8_t divBit;
	for (int i = 0; i < tCycles; i++)
	{
		//div update
		DIVRegister++;
		_mem[TIMER_DIV_LOC] = DIVRegister >> 8;

		//tima update
		if (TIMAOverflowCooldown > 0)
		{
			//tima reset to tma
			TIMAOverflowCooldown--;

			if (TIMAOverflowCooldown == 0)
			{
				_mem[TIMER_TIMA_LOC] = _mem[TIMER_TMA_LOC];
				int_timerOverflow(1);
			}
		}
		else
		{
			switch (_mem[TIMER_TAC_LOC] & 0b11)
			{
			case 0b00:
				divBit = (DIVRegister >> 9) & 1;
				break;
			case 0b01:
				divBit = (DIVRegister >> 3) & 1;
				break;
			case 0b10:
				divBit = (DIVRegister >> 5) & 1;
				break;
			case 0b11:
				divBit = (DIVRegister >> 7) & 1;
				break;
			default:
				break;
			}
			uint8_t andResult = divBit & ((_mem[TIMER_TAC_LOC] >> 2) & 1);
			if (andResult == 0 && _fallingEdgeTimerDetection == 1)
			{
				//check for TIMA overflow
				if (_mem[TIMER_TIMA_LOC] == 0xff)
				{
					TIMAOverflowCooldown = 4;
					_mem[TIMER_TIMA_LOC] = 0;
				}
				else
				{
					//update TIMA
					_mem[TIMER_TIMA_LOC] += 1;
				}
			}
			_fallingEdgeTimerDetection = andResult; //storing result for next cycle
		}
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
}

void CPU::update()
{
	int currentFrameCycles = 0;

	while (currentFrameCycles < CLOCK_CYCLES_PER_FRAME)
	{

		int cycles = ExecuteNextInstruction();

		currentFrameCycles += cycles;

		updateTimers(cycles); //update timers based on cycles

		//update ppu based on cycles

		if (!_opcodeFactory.getPrefix() && !systemHalted)
		{
			writeCurrentStateToLogFile();
			times++;
			//std::cout << times << std::endl;
		}

		if (_mem.IME() == 1)
		{
			currentFrameCycles += handleInterrupts(); //do interrupts
		}
	}
	//ppu render screen
}
