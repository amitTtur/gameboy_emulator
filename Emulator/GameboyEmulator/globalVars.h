#pragma once
#ifndef GLOBALVARS_H
#define GLOBALVARS_H
#include <cstdint>

#include <windows.h>


//this class is used to prevent include circles and dependency issues.
class globalVars
{
public:

	//system running
	static bool running();
	static void running(bool b);

	//stat register
	static bool statDoOperation();
	static void statDoOperation(bool b);

	//unique LD instruction flag
	static bool specialLdMove();
	static void specialLdMove(bool b);

	//systems DIV register - main timer
	static uint16_t DIVRegister();
	static void DIVRegister(uint16_t n);

	//used to keep the timer system updated between frames
	static uint8_t TIMAOverflowCooldown();
	static void TIMAOverflowCooldown(uint8_t n);

	//halt bug flag
	static bool haltBug();
	static void haltBug(bool b);

	//system halted
	static bool systemHalted();
	static void systemHalted(bool b);

	//cpu vram sccess flag
	static bool vramAccess();
	static void vramAccess(bool b);

	//cpu oam axxess flag
	static bool oamAccess();
	static void oamAccess(bool b);

	//dma instruction called flag
	static bool dma();
	static void dma(bool b);

	//input pad state
	static uint8_t padState();
	static void padState(uint8_t val);

	//mbc rumble flag
	static bool rumble();
	static void rumble(bool b);

	//mbc3 timer variables
	static void canIncreaseMbc3Timer(bool val);
	static bool canIncreaseMbc3Timer();
	static int getMBC3Timer();
	static void setMBC3Timer(int val);
	static int clock;

	//debugging
	static int times;


	/*
	set console text color to:
	0- black
	1- blue
	2- green
	4- red
	7- white
	14- yellow
	*/
	static void setColor(int color);

private:

	static bool _running;
	static bool _statDoOperation;
	static bool _specialLdMove;
	static uint16_t _DIVRegister;
	static uint8_t _TIMAOverflowCooldown;
	static bool _haltBug;
	static bool _systemHalted;
	static bool _vramAccess;
	static bool _oamAccess;
	static bool _dma;
	static uint8_t _padState;
	static bool _rumble;
	static int _mbc3Timer;
	static bool _canIncreaseMbc3Timer;
};
#endif