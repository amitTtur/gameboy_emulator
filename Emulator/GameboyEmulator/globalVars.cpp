#include "globalVars.h"

bool globalVars::_running = true;
bool globalVars::_statDoOperation;
bool globalVars::_specialLdMove;
uint16_t globalVars::_DIVRegister;
uint8_t globalVars::_TIMAOverflowCooldown;
bool globalVars::_systemHalted;
bool globalVars::_haltBug;
bool globalVars::_vramAccess = true;
bool globalVars::_oamAccess = true;
bool globalVars::_dma = false;
bool globalVars::_rumble = false;
uint8_t globalVars::_padState = 0;
int globalVars::times = 0;
int globalVars::clock = 0;
int globalVars::_mbc3Timer = 0;
bool globalVars::_canIncreaseMbc3Timer=false;


bool globalVars::running()
{
	return _running;
}

void globalVars::running(bool b)
{
	_running = b;
}

bool globalVars::statDoOperation()
{
	return _statDoOperation;
}

void globalVars::statDoOperation(bool b)
{
	_statDoOperation = b;
}

bool globalVars::specialLdMove()
{
	return _specialLdMove;
}

void globalVars::specialLdMove(bool b)
{
	_specialLdMove = b;
}

uint16_t globalVars::DIVRegister()
{
	return _DIVRegister;
}

void globalVars::DIVRegister(uint16_t n)
{
	_DIVRegister = n;
}

uint8_t globalVars::TIMAOverflowCooldown()
{
	return _TIMAOverflowCooldown;
}

void globalVars::TIMAOverflowCooldown(uint8_t n)
{
	_TIMAOverflowCooldown = n;
}

bool globalVars::haltBug()
{
	return _haltBug;
}

void globalVars::haltBug(bool b)
{
	_haltBug = b;
}

bool globalVars::systemHalted()
{
	return _systemHalted;
}

void globalVars::systemHalted(bool b)
{
	_systemHalted = b;
}

bool globalVars::vramAccess()
{
	return _vramAccess;
}

void globalVars::vramAccess(bool b)
{
	_vramAccess = b;
}

bool globalVars::oamAccess()
{
	return _oamAccess;
}

void globalVars::oamAccess(bool b)
{
	_oamAccess = b;
}

void globalVars::dma(bool b)
{
	_dma = b;
}

uint8_t globalVars::padState()
{
	return _padState;
}

void globalVars::padState(uint8_t val)
{
	_padState = val;
}

bool globalVars::rumble()
{
	return _rumble;
}

void globalVars::rumble(bool b)
{
	_rumble = b;
}

bool globalVars::canIncreaseMbc3Timer()
{
	return _canIncreaseMbc3Timer;
}

void globalVars::canIncreaseMbc3Timer(bool val)
{
	_canIncreaseMbc3Timer = val;
}

void globalVars::setColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int globalVars::getMBC3Timer()
{
	return _mbc3Timer;
}

void globalVars::setMBC3Timer(int val)
{
	_mbc3Timer = val;
}

bool globalVars::dma()
{
	return _dma;
}
