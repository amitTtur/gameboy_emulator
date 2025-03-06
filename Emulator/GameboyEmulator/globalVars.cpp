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
int globalVars::times = 0;

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

bool globalVars::dma()
{
	return _dma;
}
