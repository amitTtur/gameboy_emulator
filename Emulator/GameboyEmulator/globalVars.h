#pragma once
#ifndef GLOBALVARS_H
#define GLOBALVARS_H
#include <cstdint>

/****** Disclaimer ********/
// before you do a global variable add extern (basicily 'global' type) to it.
// if you want the variable to hold a first value put it in the cpp file
/**************************/

/* so other class would have access to them */

class globalVars
{
public:

	static bool running();
	static void running(bool b);

	static bool statDoOperation();
	static void statDoOperation(bool b);

	static bool specialLdMove();
	static void specialLdMove(bool b);

	static uint16_t DIVRegister();
	static void DIVRegister(uint16_t n);

	static uint8_t TIMAOverflowCooldown();
	static void TIMAOverflowCooldown(uint8_t n);

	static bool haltBug();
	static void haltBug(bool b);

	static bool systemHalted();
	static void systemHalted(bool b);

	static bool vramAccess();
	static void vramAccess(bool b);

	static bool oamAccess();
	static void oamAccess(bool b);


	static bool dma();
	static void dma(bool b);

	static int times;

private:

	static bool _running;

	static bool _statDoOperation; // if _isMemoryOperation the mbc determined if the opcode should do its thing. ---
	// example, if a write opreation occurs at 0x0 ( gb memory ) , it would change the registers at the mbc
	// but the write would not be allowed because 0x0 - 0x7FFF is read only;


	// an ld that is made to change the registers at 0xFFXX
	// see opcodes 0xE0 0xF0 0xE2 0xF2
	// those ba
	static bool _specialLdMove;


	static uint16_t _DIVRegister; //system clock

	// amit backwards ehehheheheheheheheh
	static uint8_t _TIMAOverflowCooldown; //used to keep the cooldown before the reset and overflow of TIMA to TMA between frames

	static bool _haltBug;

	static bool _systemHalted;

	// ppu allow, false no ,true yes
	// oam means there is a limited space that the cpu can get in vram, but
	// if vrram is on the oam is also locked :)
	static bool _vramAccess;
	static bool _oamAccess;

	static bool _dma;
};
#endif