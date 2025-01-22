#pragma once
#ifndef GLOBALVARS_H
#define GLOBALVARS_H
#include <cstdint>

/****** Disclaimer ********/
// before you do a global variable add extern (basicily 'global' type) to it.
// if you want the variable to hold a first value put it in the cpp file
/**************************/

/* so other class would have access to them */
extern bool statDoOperation; // if _isMemoryOperation the mbc determined if the opcode should do its thing. ---
// example, if a write opreation occurs at 0x0 ( gb memory ) , it would change the registers at the mbc
// but the write would not be allowed because 0x0 - 0x7FFF is read only;


// an ld that is made to change the registers at 0xFFXX
// see opcodes 0xE0 0xF0 0xE2 0xF2
// those ba
extern bool specialLdMove;


extern uint16_t DIVRegister; //system clock

// amit backwards ehehheheheheheheheh
extern uint8_t TIMAOverflowCooldown; //used to keep the cooldown before the reset and overflow of TIMA to TMA between frames

extern bool systemHalted;

#endif